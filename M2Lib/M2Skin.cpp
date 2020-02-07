#include "M2Skin.h"
#include "M2.h"
#include "M2Element.h"
#include "Logger.h"
#include "Shaders.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <list>
#include <assert.h>
#include <unordered_map>
#include "StringHelpers.h"
#include <filesystem>
#include "VectorMath.h"

using namespace M2Lib::M2SkinElement;

M2Lib::EError M2Lib::M2Skin::Load(wchar_t const* FileName)
{
	if (!FileName)
		return EError_FailedToLoadSKIN_NoFileSpecified;

	_FileName = FileName;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::binary | std::ios::in);
	if (FileStream.fail())
		return EError_FailedToLoadSKIN_CouldNotOpenFile;

	sLogger.LogInfo(L"Loading skin at %s", FileName);

	// find file size
	FileStream.seekg(0, std::ios::end);
	uint32_t FileSize = (uint32_t)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	// load header
	uint32_t HeaderSize = pM2->GetExpansion() >= Expansion::Cataclysm ? sizeof(Header) : 48;
	FileStream.read((char*)&Header, HeaderSize);

	// check header
	if (Header.ID[0] != 'S' || Header.ID[1] != 'K' || Header.ID[2] != 'I' || Header.ID[3] != 'N')
		return EError_FailedToLoadSKIN_FileMissingOrCorrupt;

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(FileSize);

	// load elements
	for (uint32_t i = 0; i < EElement__CountM2Skin__; ++i)
	{
		if (!Elements[i].Load(FileStream, 0))
			return EError_FailedToLoadSKIN_FileMissingOrCorrupt;
	}

	// print info
	//PrintInfo();

	// done
	return EError_OK;
}

M2Lib::EError M2Lib::M2Skin::Save(const wchar_t* FileName)
{
	auto directory = std::filesystem::path(FileName).parent_path();
	if (!std::filesystem::is_directory(directory) && !std::filesystem::create_directories(directory))
	{
		sLogger.LogError(L"Failed to write to directory '%s'", directory.wstring().c_str());

		return EError_FailedToSaveM2;
	}

	sLogger.LogInfo(L"Saving skin to %s", FileName);

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::binary | std::ios::out | std::ios::trunc);

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// save header
	uint32_t HeaderSize = pM2->GetExpansion() >= Expansion::Cataclysm ? sizeof(Header) : 48;
	FileStream.write((char*)&Header, HeaderSize);

	// save elements
	for (uint32_t i = 0; i != EElement__CountM2Skin__; i++)
	{
		if (!Elements[i].Save(FileStream, 0))
			return EError_FailedToSaveSKIN;
	}

	// close file stream
	FileStream.close();

	return EError_OK;
}

void M2Lib::M2Skin::BuildVertexBoneIndices()
{
	CVertex* VertexList = pM2->Elements[M2Element::EElement_Vertex].as<CVertex>();

	uint16_t* BoneLookupList = pM2->Elements[M2Element::EElement_SkinnedBoneLookup].as<uint16_t>();

	uint32_t VertexLookupListLength = Elements[EElement_VertexLookup].Count;
	uint16_t* VertexLookupList = Elements[EElement_VertexLookup].as<uint16_t>();

	Elements[EElement_BoneIndices].SetDataSize(VertexLookupListLength, VertexLookupListLength * sizeof(CElement_BoneIndices), false);
	CElement_BoneIndices* BoneIndexList = Elements[EElement_BoneIndices].as<CElement_BoneIndices>();

	uint32_t SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (uint32_t i = 0; i < VertexLookupListLength; ++i)
		BoneIndexList[i].Clear();

	for (uint32_t iSubMesh = 0; iSubMesh < SubMeshListLength; ++iSubMesh)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		uint32_t MaxBonesPerVertex = 0;

		uint32_t SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
		for (uint32_t j = SubMesh.VertexStart; j < SubMeshVertexEnd; ++j)
		{
			CVertex const& Vertex = VertexList[VertexLookupList[j]];

			for (int i = 0; i < BONES_PER_VERTEX; ++i)
			{
				auto res = m_ReverseBoneLookup(Vertex.BoneIndices[i], &BoneLookupList[SubMesh.BoneStart], SubMesh.BoneCount);
				if (Vertex.BoneWeights[i] && res == -1)
				{
					sLogger.LogError(L"%u/%u Bone index = %u, Submesh.ID = %u", iSubMesh, SubMeshListLength, Vertex.BoneIndices[i], SubMesh.ID);
					sLogger.LogError(L"%u/%u Submesh.BoneStart = %u, Submesh.BoneCount = %u", iSubMesh, SubMeshListLength, SubMesh.BoneStart, SubMesh.BoneCount);
					m2lib_assert(false);
				}
				BoneIndexList[j].BoneIndices[i] = Vertex.BoneWeights[i] ? res : i;
			}

			uint32_t MaxBonesPerThisVertex = 0;
			for (int i = 0; i < BONES_PER_VERTEX; ++i)
				if (Vertex.BoneWeights[i] > 0)
					++MaxBonesPerThisVertex;

			if (MaxBonesPerThisVertex > MaxBonesPerVertex)
				MaxBonesPerVertex = MaxBonesPerThisVertex;
		}

		SubMesh.MaxBonesPerVertex = MaxBonesPerVertex;
	}
}

void M2Lib::M2Skin::BuildBoundingData()
{
	CVertex* VertexList = pM2->Elements[M2Element::EElement_Vertex].as<CVertex>();
	uint16_t* VertexLookupList = Elements[EElement_VertexLookup].as<uint16_t>();

	uint32_t SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (uint32_t iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		if (SubMesh.VertexCount)
		{
			std::vector<CVertex> vertices;
			uint32_t SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
			for (uint32_t j = SubMesh.VertexStart; j < SubMeshVertexEnd; j++)
			{
				vertices.push_back(VertexList[VertexLookupList[j]]);
			}

			BoundaryData boundary;
			boundary.Calculate(vertices);

			SubMesh.CenterMass = boundary.CenterMass;
			SubMesh.SortCenter = boundary.SortCenter;
			SubMesh.SortRadius = boundary.SortRadius;
		}
	}
}

void M2Lib::M2Skin::BuildMaxBones()
{
	CVertex* VertexList = pM2->Elements[M2Element::EElement_Vertex].as<CVertex>();

	uint32_t SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (uint32_t iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		SubMesh.MaxBonesPerVertex = 0;

		uint32_t SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
		for (uint32_t j = SubMesh.VertexStart; j < SubMeshVertexEnd; j++)
		{
			uint32_t ThisVertexBoneCount = 0;
			if (VertexList[j].BoneIndices[0] != 0)
			{
				ThisVertexBoneCount++;
			}
			if (VertexList[j].BoneIndices[1] != 0)
			{
				ThisVertexBoneCount++;
			}
			if (VertexList[j].BoneIndices[2] != 0)
			{
				ThisVertexBoneCount++;
			}
			if (VertexList[j].BoneIndices[3] != 0)
			{
				ThisVertexBoneCount++;
			}

			if (SubMesh.MaxBonesPerVertex < ThisVertexBoneCount)
			{
				SubMesh.MaxBonesPerVertex = ThisVertexBoneCount;
				if (SubMesh.MaxBonesPerVertex == 4)
				{
					break;
				}
			}
		}
	}
}

void M2Lib::M2Skin::CopyMaterials(M2Skin* pOther)
{
	std::vector< CElement_Material > NewMaterialList;
	std::vector< CElement_Flags > NewFlagsList;

	uint32_t SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (uint32_t iSubMesh = 0; iSubMesh < SubMeshListLength; ++iSubMesh)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		m2lib_assert(iSubMesh < ExtraDataBySubmeshIndex.size());
		auto comparisonData = ExtraDataBySubmeshIndex[iSubMesh];

		int32_t SubMeshOtherIndex;
		CElement_SubMesh* SubMeshOther = pOther->GetSubMesh(*comparisonData, SubMeshOtherIndex);
		m2lib_assert(SubMeshOther);

		SubMesh.CenterBoneIndex = SubMeshOther->CenterBoneIndex;
		// copy level from original mesh
		//SubMesh.Level = SubMeshOther->Level;

		std::vector< CElement_Material* > SubMeshOtherMaterialList;
		pOther->GetSubMeshMaterials(SubMeshOtherIndex, SubMeshOtherMaterialList);
		for (uint32_t iSubMeshMaterialOther = 0; iSubMeshMaterialOther < SubMeshOtherMaterialList.size(); iSubMeshMaterialOther++)
		{
			CElement_Material NewMaterial = *SubMeshOtherMaterialList[iSubMeshMaterialOther];
			NewMaterial.iSubMesh = iSubMesh;
			NewMaterial.iSubMesh2 = 0;
			/*for (int i = 0; i < SubMeshOtherMaterialList.size(); ++i)
			{
				if (SubMeshOtherMaterialList[i]->iSubMesh == iSubMesh)
				NewMaterial.iSubMesh2 = SubMeshOtherMaterialList[i]->iSubMesh2;
			}*/
			NewMaterialList.push_back(NewMaterial);
		}

		std::vector< CElement_Flags* > SubMeshOtherFlagsList;
		pOther->GetSubMeshFlags(SubMeshOtherIndex, SubMeshOtherFlagsList);
		for (uint32_t iSubMeshFlagsOther = 0; iSubMeshFlagsOther < SubMeshOtherFlagsList.size(); iSubMeshFlagsOther++)
		{
			CElement_Flags NewFlags = *SubMeshOtherFlagsList[iSubMeshFlagsOther];
			NewFlags.iSubMesh = iSubMesh;
			NewFlagsList.push_back(NewFlags);
		}
	}

	// copy new material list to element
	Elements[EElement_Material].SetDataSize(NewMaterialList.size(), sizeof(CElement_Material) * NewMaterialList.size(), false);
	memcpy(Elements[EElement_Material].Data.data(), &NewMaterialList[0], sizeof(CElement_Material) * NewMaterialList.size());

	// copy new flags list to element
	Elements[EElement_Flags].SetDataSize(NewFlagsList.size(), sizeof(CElement_Flags) * NewFlagsList.size(), false);
	memcpy(Elements[EElement_Flags].Data.data(), &NewFlagsList[0], sizeof(CElement_Flags) * NewFlagsList.size());
}

bool SortSubMeshesComparisonFunction(M2Lib::M2SkinElement::CElement_SubMesh* A, M2Lib::M2SkinElement::CElement_SubMesh* B)
{
	float ScoreA = A->ID + (0.999f - (A->VertexCount / (float)0xFFFF));
	float ScoreB = B->ID + (0.999f - (B->VertexCount / (float)0xFFFF));
	return (ScoreA < ScoreB);
}

void M2Lib::M2Skin::SortSubMeshes()
{
	uint32_t SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	// sort the list
	std::list< CElement_SubMesh* > SubMeshListSorted;
	for (uint32_t iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		SubMeshListSorted.push_back(&SubMeshList[iSubMesh]);
	}
	SubMeshListSorted.sort(SortSubMeshesComparisonFunction);

	// build index remap list
	uint32_t* SubMeshReMapList = new uint32_t[SubMeshListLength];
	for (uint32_t i = 0; i < SubMeshListLength; i++)
	{
		std::list< CElement_SubMesh* >::iterator it = SubMeshListSorted.begin();
		for (uint32_t j = 0; j < SubMeshListLength; j++)
		{
			if (&SubMeshList[i] == *it)
			{
				SubMeshReMapList[i] = j;
			}
			it++;
		}
	}

	// remap material sub mesh indices
	uint32_t MaterialListLength = Elements[EElement_Material].Count;
	CElement_Material* MaterialList = Elements[EElement_Material].as<CElement_Material>();
	for (uint32_t i = 0; i < MaterialListLength; i++)
	{
		MaterialList[i].iSubMesh = SubMeshReMapList[MaterialList[i].iSubMesh];
		MaterialList[i].iSubMesh2 = SubMeshReMapList[MaterialList[i].iSubMesh2];
	}
	delete[] SubMeshReMapList;

	// copy sorted result
	CElement_SubMesh* NewSubMeshList = new CElement_SubMesh[SubMeshListLength];
	std::list< CElement_SubMesh* >::iterator SubMeshListSortedIterator = SubMeshListSorted.begin();
	for (uint32_t iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		NewSubMeshList[iSubMesh] = *(*SubMeshListSortedIterator);
		SubMeshListSortedIterator++;
	}
	memcpy(Elements[EElement_SubMesh].Data.data(), NewSubMeshList, Elements[EElement_SubMesh].Data.size());
	delete[] NewSubMeshList;
}


M2Lib::M2SkinElement::CElement_SubMesh* M2Lib::M2Skin::GetSubMesh(SubmeshExtraData const& TargetSubMeshData, int32_t& SubMeshIndexOut)
{
	uint32_t SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	if (TargetSubMeshData.OriginalSubmeshIndex >= 0)
	{
		m2lib_assert((uint32_t)TargetSubMeshData.OriginalSubmeshIndex < SubMeshListLength);

		SubMeshIndexOut = TargetSubMeshData.OriginalSubmeshIndex;
		return &SubMeshList[SubMeshIndexOut];
	}

	float DeltaMin = 0.0f;
	int32_t ClosestMatch = -1;
	SubMeshIndexOut = -1;

	uint16_t ID = TargetSubMeshData.ID;
	C3Vector const& SortCenter = TargetSubMeshData.Boundary.SortCenter;
	C3Vector const& CenterMass = TargetSubMeshData.Boundary.CenterMass;

	for (uint32_t i = 0; i < SubMeshListLength; ++i)
	{
		if (SubMeshList[i].ID == ID)
		{
			C3Vector SortDelta = SubMeshList[i].SortCenter - SortCenter;
			C3Vector BoundingDelta = SubMeshList[i].CenterMass - CenterMass;
			float Delta = SortDelta.Length() + BoundingDelta.Length();

			if (ClosestMatch == -1 || Delta < DeltaMin)
			{
				DeltaMin = Delta;
				ClosestMatch = i;
			}
		}
	}

	if (ClosestMatch >= 0)
	{
		SubMeshIndexOut = ClosestMatch;
		return &SubMeshList[ClosestMatch];
	}

	return 0;
}


void M2Lib::M2Skin::GetSubMeshMaterials(uint32_t SubMeshIndex, std::vector< CElement_Material* >& Result)
{
	CElement_Material* MaterialList = Elements[EElement_Material].as<CElement_Material>();
	for (uint32_t iMaterial = 0; iMaterial < Header.nMaterial; iMaterial++)
	{
		if (MaterialList[iMaterial].iSubMesh == SubMeshIndex)
		{
			Result.push_back(&MaterialList[iMaterial]);
		}
	}
}


void M2Lib::M2Skin::GetSubMeshFlags(uint32_t SubMeshIndex, std::vector< CElement_Flags* >& Result)
{
	CElement_Flags* FlagsList = Elements[EElement_Flags].as<CElement_Flags>();
	for (uint32_t iFlags = 0; iFlags < Header.nFlags; iFlags++)
	{
		if (FlagsList[iFlags].iSubMesh == SubMeshIndex)
		{
			Result.push_back(&FlagsList[iFlags]);
		}
	}
}


bool M2Lib::M2Skin::PrintInfo()
{
	std::wstring FileOut = std::filesystem::path(_FileName).replace_extension("txt");

	std::fstream FileStream;
	FileStream.open(FileOut.c_str(), std::ios::out | std::ios::trunc);

	int32_t MaxBones = 0;
	CElement_SubMesh* SubMeshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	for (uint32_t i = 0; i < Header.nSubMesh; i++)
	{
		CElement_SubMesh& Subset = SubMeshes[i];
		if (Subset.BoneCount > MaxBones)
		{
			MaxBones = Subset.BoneCount;
		}
	}
	FileStream << "MaxBones:      " << MaxBones << std::endl;
	FileStream << std::endl;

	FileStream << "nVertex:       " << Header.nVertex << std::endl;
	FileStream << "oVertex:       " << Header.oVertex << std::endl;
	FileStream << std::endl;

	FileStream << "nTriangleIndex:        " << Header.nTriangleIndex << std::endl;
	FileStream << "oTriangleIndex:        " << Header.oTriangleIndex << std::endl;
	FileStream << std::endl;

	FileStream << "nBoneIndices:  " << Header.nBoneIndices << std::endl;
	FileStream << "oBoneIndices:  " << Header.oBoneIndices << std::endl;
	FileStream << std::endl;

	FileStream << "nSubMesh:       " << Header.nSubMesh << std::endl;
	FileStream << "oSubMesh:       " << Header.oSubMesh << std::endl;
	FileStream << std::endl;

	FileStream << "nMaterial:     " << Header.nMaterial << std::endl;
	FileStream << "oMaterial:     " << Header.oMaterial << std::endl;
	FileStream << std::endl;

	FileStream << "BoneCountMax:  " << Header.BoneCountMax << std::endl;
	FileStream << std::endl;

	FileStream << "nFlags:        " << Header.nFlags << std::endl;
	FileStream << "oFlags:        " << Header.oFlags << std::endl;
	FileStream << std::endl;

	FileStream << "Unknown2:      " << Header.Unknown2 << std::endl;
	FileStream << "Unknown3:      " << Header.Unknown3 << std::endl;
	FileStream << std::endl;


	FileStream << "//" << std::endl;
	FileStream << "// SUBSETS" << std::endl;
	FileStream << std::endl;
	//CElement_SubMesh* SubMeshes = (CElement_SubMesh*)Elements[EElement_SubMesh].Data;
	for (uint32_t i = 0; i < Header.nSubMesh; i++)
	{
		CElement_SubMesh& Subset = SubMeshes[i];
		FileStream << "[" << i << "]" << std::endl;
		FileStream << "ID:           " << Subset.ID << std::endl;
		FileStream << "Level:        " << Subset.Level << std::endl;
		FileStream << "VertexStart:  " << Subset.VertexStart << std::endl;
		FileStream << "VertexCount:  " << Subset.VertexCount << std::endl;
		FileStream << "TriangleIndexStart:   " << Subset.TriangleIndexStart << std::endl;
		FileStream << "TriangleIndexCount:   " << Subset.TriangleIndexCount << std::endl;
		FileStream << "BoneCount:    " << Subset.BoneCount << std::endl;
		FileStream << "BoneStart:    " << Subset.BoneStart << std::endl;
		FileStream << "MaxBonesPerVertex:     " << Subset.MaxBonesPerVertex << std::endl;
		FileStream << "SortTriangleIndex:     " << Subset.CenterBoneIndex << std::endl;
		FileStream << "CenterMass:   " << "( " << Subset.CenterMass.X << ", " << Subset.CenterMass.Y << ", " << Subset.CenterMass.Z << " ) " << std::endl;
		FileStream << "SortCenter:   " << "( " << Subset.SortCenter.X << ", " << Subset.SortCenter.Y << ", " << Subset.SortCenter.Z << " ) " << std::endl;
		FileStream << "SortRadius:   " << Subset.SortRadius << std::endl;
		FileStream << std::endl;
	}


	FileStream << "//" << std::endl;
	FileStream << "// FLAGS" << std::endl;
	FileStream << std::endl;
	for (uint32_t i = 0; i < Header.nFlags; i++)
	{
		CElement_Flags& Flags = Elements[EElement_Flags].as<CElement_Flags>()[i];
		FileStream << "[" << i << "]    SubmeshID: " << SubMeshes[Flags.iSubMesh].ID << std::endl;
		FileStream << "Flags1:     " << Flags.Flags1 << std::endl;
		FileStream << "Unknown1:   " << Flags.Unknown1 << std::endl;
		FileStream << "iSubMesh:    " << Flags.iSubMesh << std::endl;
		FileStream << "TextureId:     " << Flags.TextureId << std::endl;
		FileStream << "ColorId:   " << Flags.ColorId << std::endl;
		FileStream << "TransparencyId:   " << Flags.TransparencyId << std::endl;
		FileStream << std::endl;
	}

    FileStream << "//" << std::endl;
    FileStream << "// MATERIALS" << std::endl;
    FileStream << std::endl;
    for (uint32_t i = 0; i < Header.nMaterial; i++)
    {
        CElement_Material& Material = Elements[EElement_Material].as<CElement_Material>()[i];
        FileStream << "Flags:     " << Material.Flags << std::endl;
        //FileStream << "shader_id:   " << Material.shader_id << std::endl;
        FileStream << "iSubMesh:    " << Material.iSubMesh << std::endl;
        FileStream << "iSubMesh2:     " << Material.iSubMesh2 << std::endl;
        FileStream << "iColor:   " << Material.iColor << std::endl;
        FileStream << "iRenderFlags:   " << Material.iRenderFlags << std::endl;

        FileStream << "layer:   " << Material.layer << std::endl;
        //FileStream << "op_count:   " << Material.op_count << std::endl;
        FileStream << "textureComboIndex:   " << Material.textureComboIndex << std::endl;
        FileStream << "textureCoordComboIndex:   " << Material.textureCoordComboIndex << std::endl;

        FileStream << "textureWeightComboIndex:   " << Material.textureWeightComboIndex << std::endl;
        FileStream << "textureTransformComboIndex:   " << Material.textureTransformComboIndex << std::endl;
        FileStream << std::endl;
    }

	FileStream.close();

	return true;
}


void M2Lib::M2Skin::m_LoadElements_CopyHeaderToElements()
{
	Elements[EElement_VertexLookup].Count = Header.nVertex;
	Elements[EElement_VertexLookup].Offset = Header.oVertex;

	Elements[EElement_TriangleIndex].Count = Header.nTriangleIndex;
	Elements[EElement_TriangleIndex].Offset = Header.oTriangleIndex;

	Elements[EElement_BoneIndices].Count = Header.nBoneIndices;
	Elements[EElement_BoneIndices].Offset = Header.oBoneIndices;

	Elements[EElement_SubMesh].Count = Header.nSubMesh;
	Elements[EElement_SubMesh].Offset = Header.oSubMesh;

	Elements[EElement_Material].Count = Header.nMaterial;
	Elements[EElement_Material].Offset = Header.oMaterial;

	if (pM2->GetExpansion() >= Expansion::Cataclysm)
	{
		Elements[EElement_Flags].Count = Header.nFlags;
		Elements[EElement_Flags].Offset = Header.oFlags;
	}
}


void M2Lib::M2Skin::m_LoadElements_FindSizes(uint32_t FileSize)
{
	for (uint32_t i = 0; i < EElement__CountM2Skin__; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Element.Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		uint32_t NextOffset = FileSize;
		for (uint32_t j = i + 1; j < EElement__CountM2Skin__; ++j)
		{
			if (Elements[j].Offset)
			{
				NextOffset = Elements[j].Offset;
				break;
			}
		}

		m2lib_assert(NextOffset >= Element.Offset && "SKIN Elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}
}

void M2Lib::M2Skin::m_SaveElements_FindOffsets()
{
	uint32_t CurrentOffset = sizeof(Header);
	CurrentOffset = (CurrentOffset + 15) & ~15;

	for (uint32_t i = 0; i < EElement__CountM2Skin__; ++i)
	{
		if (!Elements[i].Data.empty())
		{
			Elements[i].Offset = CurrentOffset;
			Elements[i].SizeOriginal = Elements[i].Data.size();
			Elements[i].OffsetOriginal = CurrentOffset;

			CurrentOffset += Elements[i].Data.size();
		}
	}
}

void M2Lib::M2Skin::m_SaveElements_CopyElementsToHeader()
{
	Header.nVertex = Elements[EElement_VertexLookup].Count;
	Header.oVertex = Elements[EElement_VertexLookup].Offset;

	Header.nTriangleIndex = Elements[EElement_TriangleIndex].Count;
	Header.oTriangleIndex = Elements[EElement_TriangleIndex].Offset;

	Header.nBoneIndices = Elements[EElement_BoneIndices].Count;
	Header.oBoneIndices = Elements[EElement_BoneIndices].Offset;

	Header.nSubMesh = Elements[EElement_SubMesh].Count;
	Header.oSubMesh = Elements[EElement_SubMesh].Offset;

	Header.nMaterial = Elements[EElement_Material].Count;
	Header.oMaterial = Elements[EElement_Material].Offset;

	Header.BoneCountMax = 0;

	if (pM2->GetExpansion() >= Expansion::Cataclysm)
	{
		Header.nFlags = Elements[EElement_Flags].Count;
		Header.oFlags = Elements[EElement_Flags].Offset;
	}

	Header.Unknown2 = 0;
	Header.Unknown3 = 0;
}

bool M2Lib::M2Skin::AddShader(uint16_t ShaderId, uint32_t const* MeshTextureIds, std::vector<uint32_t> const& MeshIndexes, TextureLookupRemap& LookupRemap)
{
	uint32_t OpCountForShader = ShaderId == TRANSPARENT_SHADER_ID ? 1 : GetOpCountForShader(ShaderId);

	auto Submeshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	auto Materials = Elements[EElement_Material].as<CElement_Material>();
	auto TextureLookup = pM2->Elements[M2Element::EElement_TextureLookup].as<M2Element::CElement_TextureLookup>();

	for (auto submeshId : MeshIndexes)
	{
		auto& SubMesh = Submeshes[submeshId];
		
		// loop through all materials to find ones assigned to our submesh
		for (uint32_t i = 0; i < Elements[EElement_Material].Count; ++i)
		{
			auto& Material = Materials[i];
			if (Material.iSubMesh != submeshId)
				continue;

			auto textureId = MeshTextureIds[0] != -1 ? MeshTextureIds[0] : TextureLookup[Material.textureComboIndex].TextureIndex;
			auto& texture = pM2->Elements[M2Element::EElement_Texture].as<M2Element::CElement_Texture>()[textureId];
			if (texture.Type == M2Element::CElement_Texture::ETextureType::Skin)
				pM2->Header.Description.Flags.flag_unk_0x80 = 0;	// fixes gloss but breaks dh tattoos

			int newLookup;

			// TODO: for now always add new lookups, this code does not work as intended
			if (true || LookupRemap.find(textureId) == LookupRemap.end())
			{
				int replaceId = -1;
				//if (pM2->Elements[M2Element::EElement_Texture].as<M2Element::CElement_Texture>()[textureId].Type == M2Element::CElement_Texture::ETextureType::Skin)
				//	replaceId = pM2->CloneTexture(textureId);

				// add new lookups successively to use with op_count
				newLookup = pM2->AddTextureLookup(replaceId != -1 ? replaceId : textureId, true);
				for (uint32_t j = 1; j < OpCountForShader; ++j)
					pM2->AddTextureLookup(MeshTextureIds[j], true);
				LookupRemap[textureId] = newLookup;
			}
			else
				newLookup = LookupRemap[textureId];

			Material.textureComboIndex = newLookup;
			Material.op_count = OpCountForShader;
			Material.shader_id = ShaderId;
		}
	}

	return true;
}

bool M2Lib::M2Skin::AddShader(uint16_t ShaderId, int16_t const* TextureTypes, std::string const* MeshTextures, std::vector<uint32_t> const& MeshIndexes, TextureLookupRemap& LookupRemap)
{
	uint32_t MeshTextureIds[MAX_SUBMESH_TEXTURES];
	uint32_t OpCountForShader = ShaderId == TRANSPARENT_SHADER_ID ? 1 : GetOpCountForShader(ShaderId);

	for (uint32_t i = 0; i < OpCountForShader; ++i)
	{
		if (TextureTypes[i] == -1)
		{
			if (i != 0)
			{
				sLogger.LogError(L"Failed to apply shader data: texture not set for op#%u (total %u ops)", i, OpCountForShader);
				return false;
			}

			MeshTextureIds[i] = -1;
			continue;
		}

		int32_t TextureId = pM2->GetTextureIndex((M2Element::CElement_Texture::ETextureType)TextureTypes[i], MeshTextures[i].c_str());
		if (TextureId == -1)
			TextureId = pM2->AddTexture((M2Element::CElement_Texture::ETextureType)TextureTypes[i], M2Element::CElement_Texture::ETextureFlags::None, MeshTextures[i].c_str());

		MeshTextureIds[i] = TextureId;
	}

	return AddShader(ShaderId, MeshTextureIds, MeshIndexes, LookupRemap);
}

std::vector<M2Lib::M2Skin::MeshInfo> M2Lib::M2Skin::GetMeshInfo()
{
	std::vector<MeshInfo> Infos(Elements[EElement_SubMesh].Count);

	auto Submeshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	auto Materials = Elements[EElement_Material].as<CElement_Material>();
	auto& TextureElement = pM2->Elements[M2Element::EElement_Texture];
	auto TextureLookup = pM2->Elements[M2Element::EElement_TextureLookup].as<M2Element::CElement_TextureLookup>();

	for (uint32_t i = 0; i < Infos.size(); ++i)
	{
		auto& Info = Infos[i];
		Info.ID = Submeshes[i].ID;
		Info.pSubMesh = &Submeshes[i];

		m2lib_assert(i < ExtraDataBySubmeshIndex.size());
		auto comparisonData = ExtraDataBySubmeshIndex[i];

		Info.Description = comparisonData->Description;

		for (uint32_t j = 0; j < Header.nMaterial; ++j)
		{
			if (Materials[j].iSubMesh != i)
				continue;

			Info.Materials.push_back(&Materials[j]);
		}

		for (auto Material : Info.Materials)
		{
			for (int16_t k = 0; k < Material->op_count; ++k)
			{
				MeshInfo::TextureInfo TextureInfo;

				auto& texture = TextureElement.as<M2Element::CElement_Texture>()[TextureLookup[Material->textureComboIndex + k].TextureIndex];
				TextureInfo.pTexture = &texture;

				if (texture.Type == M2Element::CElement_Texture::ETextureType::Final_Hardcoded)
					TextureInfo.Name = pM2->GetTexturePath(TextureLookup[Material->textureComboIndex + k].TextureIndex);

				Info.Textures.push_back(TextureInfo);
			}
		}
	}

	return Infos;
}

void M2Lib::M2Skin::CopyMaterial(uint32_t SrcMeshIndex, uint32_t DstMeshIndex)
{
	auto Meshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	auto Materials = Elements[EElement_Material].as<CElement_Material>();
	
	CElement_Material* SrcMaterial = NULL;
	for (uint32_t i = 0; i < Header.nMaterial; ++i)
	{
		if (Materials[i].iSubMesh != SrcMeshIndex)
			continue;

		SrcMaterial = &Materials[i];
	}

	if (!SrcMaterial)
		return;

	for (uint32_t i = 0; i < Header.nMaterial; ++i)
	{
		if (Materials[i].iSubMesh != DstMeshIndex)
			continue;

		auto& DstMaterial = Materials[i];
		DstMaterial.Flags = SrcMaterial->Flags;
		DstMaterial.shader_id = SrcMaterial->shader_id;
		DstMaterial.iColor = SrcMaterial->iColor;
		DstMaterial.iRenderFlags = SrcMaterial->iRenderFlags;
		DstMaterial.layer = SrcMaterial->layer;
		DstMaterial.op_count = SrcMaterial->op_count;
		DstMaterial.textureComboIndex = SrcMaterial->textureComboIndex;
		DstMaterial.textureCoordComboIndex = SrcMaterial->textureCoordComboIndex;
		DstMaterial.textureWeightComboIndex = SrcMaterial->textureWeightComboIndex;
		DstMaterial.textureTransformComboIndex = SrcMaterial->textureTransformComboIndex;
	}
}
