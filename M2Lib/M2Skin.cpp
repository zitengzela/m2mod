#include "M2Skin.h"
#include "M2.h"
#include "M2Element.h"
#include "FileSystem.h"
#include "Logger.h"
#include "Shaders.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <list>
#include <assert.h>
#include <unordered_map>

using namespace M2Lib::M2SkinElement;

M2Lib::EError M2Lib::M2Skin::Load(Char16 const* FileName)
{
	if (!FileName)
		return EError_FailedToLoadSKIN_NoFileSpecified;

	_FileName = FileName;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::binary | std::ios::in);
	if (FileStream.fail())
		return EError_FailedToLoadSKIN_CouldNotOpenFile;

	sLogger.LogInfo("Loading skin at %s", WStringToString(FileName).c_str());

	// find file size
	FileStream.seekg(0, std::ios::end);
	UInt32 FileSize = (UInt32)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	// load header
	UInt32 HeaderSize = pM2->GetExpansion() >= Expansion::Cataclysm ? sizeof(Header) : 48;
	FileStream.read((char*)&Header, HeaderSize);

	// check header
	if (Header.ID[0] != 'S' || Header.ID[1] != 'K' || Header.ID[2] != 'I' || Header.ID[3] != 'N')
		return EError_FailedToLoadSKIN_FileMissingOrCorrupt;

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(FileSize);

	// load elements
	for (UInt32 i = 0; i < EElement__Count__; ++i)
	{
		if (!Elements[i].Load(FileStream, 0))
			return EError_FailedToLoadSKIN_FileMissingOrCorrupt;
	}

	// print info
	//PrintInfo();

	// done
	return EError_OK;
}

M2Lib::EError M2Lib::M2Skin::Save(const Char16* FileName)
{
	auto directory = FileSystemW::GetParentDirectory(FileName);
	if (!FileSystemW::IsDirectory(directory) && !FileSystemW::CreateDirectory(directory))
		return EError_FailedToSaveM2;

	sLogger.LogInfo("Saving skin to %s", WStringToString(FileName).c_str());

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::binary | std::ios::out | std::ios::trunc);

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// save header
	UInt32 HeaderSize = pM2->GetExpansion() >= Expansion::Cataclysm ? sizeof(Header) : 48;
	FileStream.write((Char8*)&Header, HeaderSize);

	// save elements
	for (UInt32 i = 0; i != EElement__Count__; i++)
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

	UInt16* BoneLookupList = pM2->Elements[M2Element::EElement_SkinnedBoneLookup].as<UInt16>();

	UInt32 VertexLookupListLength = Elements[EElement_VertexLookup].Count;
	UInt16* VertexLookupList = Elements[EElement_VertexLookup].as<UInt16>();

	Elements[EElement_BoneIndices].SetDataSize(VertexLookupListLength, VertexLookupListLength * sizeof(CElement_BoneIndices), false);
	CElement_BoneIndices* BoneIndexList = Elements[EElement_BoneIndices].as<CElement_BoneIndices>();

	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (UInt32 i = 0; i < VertexLookupListLength; ++i)
		BoneIndexList[i].Clear();

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; ++iSubMesh)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		UInt32 MaxBonesPerVertex = 0;

		UInt32 SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
		for (UInt32 j = SubMesh.VertexStart; j < SubMeshVertexEnd; ++j)
		{
			CVertex const& Vertex = VertexList[VertexLookupList[j]];

			for (int i = 0; i < BONES_PER_VERTEX; ++i)
			{
				auto res = m_ReverseBoneLookup(Vertex.BoneIndices[i], &BoneLookupList[SubMesh.BoneStart], SubMesh.BoneCount);
				if (Vertex.BoneWeights[i] && res == -1)
				{
					sLogger.LogError("%u/%u Bone index = %u, Submesh.ID = %u", iSubMesh, SubMeshListLength, Vertex.BoneIndices[i], SubMesh.ID);
					sLogger.LogError("%u/%u Submesh.BoneStart = %u, Submesh.BoneCount = %u", iSubMesh, SubMeshListLength, SubMesh.BoneStart, SubMesh.BoneCount);
					assert(false);
				}
				BoneIndexList[j].BoneIndices[i] = Vertex.BoneWeights[i] ? res : i;
			}

			UInt32 MaxBonesPerThisVertex = 0;
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
	UInt16* VertexLookupList = Elements[EElement_VertexLookup].as<UInt16>();

	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		if (SubMesh.VertexCount)
		{
			std::vector<CVertex> vertices;
			UInt32 SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
			for (UInt32 j = SubMesh.VertexStart; j < SubMeshVertexEnd; j++)
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

	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		SubMesh.MaxBonesPerVertex = 0;

		UInt32 SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
		for (UInt32 j = SubMesh.VertexStart; j < SubMeshVertexEnd; j++)
		{
			UInt32 ThisVertexBoneCount = 0;
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

	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; ++iSubMesh)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		assert(iSubMesh < ExtraDataBySubmeshIndex.size());
		auto comparisonData = ExtraDataBySubmeshIndex[iSubMesh];

		SInt32 SubMeshOtherIndex;
		CElement_SubMesh* SubMeshOther = pOther->GetSubMesh(*comparisonData, SubMeshOtherIndex);
		assert(SubMeshOther);

		SubMesh.CenterBoneIndex = SubMeshOther->CenterBoneIndex;
		// copy level from original mesh
		//SubMesh.Level = SubMeshOther->Level;

		std::vector< CElement_Material* > SubMeshOtherMaterialList;
		pOther->GetSubMeshMaterials(SubMeshOtherIndex, SubMeshOtherMaterialList);
		for (UInt32 iSubMeshMaterialOther = 0; iSubMeshMaterialOther < SubMeshOtherMaterialList.size(); iSubMeshMaterialOther++)
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
		for (UInt32 iSubMeshFlagsOther = 0; iSubMeshFlagsOther < SubMeshOtherFlagsList.size(); iSubMeshFlagsOther++)
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
	Float32 ScoreA = A->ID + (0.999f - (A->VertexCount / (Float32)0xFFFF));
	Float32 ScoreB = B->ID + (0.999f - (B->VertexCount / (Float32)0xFFFF));
	return (ScoreA < ScoreB);
}

void M2Lib::M2Skin::SortSubMeshes()
{
	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	// sort the list
	std::list< CElement_SubMesh* > SubMeshListSorted;
	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		SubMeshListSorted.push_back(&SubMeshList[iSubMesh]);
	}
	SubMeshListSorted.sort(SortSubMeshesComparisonFunction);

	// build index remap list
	UInt32* SubMeshReMapList = new UInt32[SubMeshListLength];
	for (UInt32 i = 0; i < SubMeshListLength; i++)
	{
		std::list< CElement_SubMesh* >::iterator it = SubMeshListSorted.begin();
		for (UInt32 j = 0; j < SubMeshListLength; j++)
		{
			if (&SubMeshList[i] == *it)
			{
				SubMeshReMapList[i] = j;
			}
			it++;
		}
	}

	// remap material sub mesh indices
	UInt32 MaterialListLength = Elements[EElement_Material].Count;
	CElement_Material* MaterialList = Elements[EElement_Material].as<CElement_Material>();
	for (UInt32 i = 0; i < MaterialListLength; i++)
	{
		MaterialList[i].iSubMesh = SubMeshReMapList[MaterialList[i].iSubMesh];
		MaterialList[i].iSubMesh2 = SubMeshReMapList[MaterialList[i].iSubMesh2];
	}
	delete[] SubMeshReMapList;

	// copy sorted result
	CElement_SubMesh* NewSubMeshList = new CElement_SubMesh[SubMeshListLength];
	std::list< CElement_SubMesh* >::iterator SubMeshListSortedIterator = SubMeshListSorted.begin();
	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		NewSubMeshList[iSubMesh] = *(*SubMeshListSortedIterator);
		SubMeshListSortedIterator++;
	}
	memcpy(Elements[EElement_SubMesh].Data.data(), NewSubMeshList, Elements[EElement_SubMesh].Data.size());
	delete[] NewSubMeshList;
}


M2Lib::M2SkinElement::CElement_SubMesh* M2Lib::M2Skin::GetSubMesh(SubmeshExtraData const& TargetSubMeshData, SInt32& SubMeshIndexOut)
{
	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	if (TargetSubMeshData.OriginalSubmeshIndex >= 0)
	{
		assert(TargetSubMeshData.OriginalSubmeshIndex < SubMeshListLength);

		SubMeshIndexOut = TargetSubMeshData.OriginalSubmeshIndex;
		return &SubMeshList[SubMeshIndexOut];
	}

	Float32 DeltaMin = 0.0f;
	SInt32 ClosestMatch = -1;
	SubMeshIndexOut = -1;

	UInt16 ID = TargetSubMeshData.ID;
	C3Vector const& SortCenter = TargetSubMeshData.Boundary.SortCenter;
	C3Vector const& CenterMass = TargetSubMeshData.Boundary.CenterMass;

	for (UInt32 i = 0; i < SubMeshListLength; ++i)
	{
		if (SubMeshList[i].ID == ID)
		{
			C3Vector SortDelta = SubMeshList[i].SortCenter - SortCenter;
			C3Vector BoundingDelta = SubMeshList[i].CenterMass - CenterMass;
			Float32 Delta = SortDelta.Length() + BoundingDelta.Length();

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


void M2Lib::M2Skin::GetSubMeshMaterials(UInt32 SubMeshIndex, std::vector< CElement_Material* >& Result)
{
	CElement_Material* MaterialList = Elements[EElement_Material].as<CElement_Material>();
	for (UInt32 iMaterial = 0; iMaterial < Header.nMaterial; iMaterial++)
	{
		if (MaterialList[iMaterial].iSubMesh == SubMeshIndex)
		{
			Result.push_back(&MaterialList[iMaterial]);
		}
	}
}


void M2Lib::M2Skin::GetSubMeshFlags(UInt32 SubMeshIndex, std::vector< CElement_Flags* >& Result)
{
	CElement_Flags* FlagsList = Elements[EElement_Flags].as<CElement_Flags>();
	for (UInt32 iFlags = 0; iFlags < Header.nFlags; iFlags++)
	{
		if (FlagsList[iFlags].iSubMesh == SubMeshIndex)
		{
			Result.push_back(&FlagsList[iFlags]);
		}
	}
}


bool M2Lib::M2Skin::PrintInfo()
{
	std::wstring FileOut = FileSystemW::GetParentDirectory(_FileName) + L"\\" + FileSystemW::GetFileName(_FileName) + L".txt";

	std::fstream FileStream;
	FileStream.open(FileOut.c_str(), std::ios::out | std::ios::trunc);

	SInt32 MaxBones = 0;
	CElement_SubMesh* SubMeshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	for (UInt32 i = 0; i < Header.nSubMesh; i++)
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
	for (UInt32 i = 0; i < Header.nSubMesh; i++)
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
	for (UInt32 i = 0; i < Header.nFlags; i++)
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
    for (UInt32 i = 0; i < Header.nMaterial; i++)
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


void M2Lib::M2Skin::m_LoadElements_FindSizes(UInt32 FileSize)
{
	for (UInt32 i = 0; i < EElement__Count__; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Element.Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = FileSize;
		for (UInt32 j = i + 1; j < EElement__Count__; ++j)
		{
			if (Elements[j].Offset)
			{
				NextOffset = Elements[j].Offset;
				break;
			}
		}

		assert(NextOffset >= Element.Offset && "SKIN Elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}
}

void M2Lib::M2Skin::m_SaveElements_FindOffsets()
{
	UInt32 CurrentOffset = sizeof(Header);
	CurrentOffset = (CurrentOffset + 15) & ~15;

	for (UInt32 i = 0; i < EElement__Count__; ++i)
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

bool M2Lib::M2Skin::AddShader(UInt16 ShaderId, UInt32 const* MeshTextureIds, std::vector<UInt32> const& MeshIndexes, TextureLookupRemap& LookupRemap)
{
	UInt32 OpCountForShader = ShaderId == TRANSPARENT_SHADER_ID ? 1 : GetOpCountForShader(ShaderId);

	auto Submeshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	auto Materials = Elements[EElement_Material].as<CElement_Material>();
	auto TextureLookup = pM2->Elements[M2Element::EElement_TextureLookup].as<M2Element::CElement_TextureLookup>();

	for (auto submeshId : MeshIndexes)
	{
		auto& SubMesh = Submeshes[submeshId];
		
		// loop through all materials to find ones assigned to our submesh
		for (UInt32 i = 0; i < Elements[EElement_Material].Count; ++i)
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
				for (UInt32 j = 1; j < OpCountForShader; ++j)
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

bool M2Lib::M2Skin::AddShader(UInt16 ShaderId, SInt16 const* TextureTypes, std::string const* MeshTextures, std::vector<UInt32> const& MeshIndexes, TextureLookupRemap& LookupRemap)
{
	UInt32 MeshTextureIds[MAX_SUBMESH_TEXTURES];
	UInt32 OpCountForShader = ShaderId == TRANSPARENT_SHADER_ID ? 1 : GetOpCountForShader(ShaderId);

	for (UInt32 i = 0; i < OpCountForShader; ++i)
	{
		if (TextureTypes[i] == -1)
		{
			if (i != 0)
			{
				sLogger.LogError("Failed to apply shader data: texture not set for op#%u (total %u ops)", i, OpCountForShader);
				return false;
			}

			MeshTextureIds[i] = -1;
			continue;
		}

		SInt32 TextureId = pM2->GetTextureIndex((M2Element::CElement_Texture::ETextureType)TextureTypes[i], MeshTextures[i].c_str());
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

	for (UInt32 i = 0; i < Infos.size(); ++i)
	{
		auto& Info = Infos[i];
		Info.ID = Submeshes[i].ID;
		Info.pSubMesh = &Submeshes[i];

		assert(i < ExtraDataBySubmeshIndex.size());
		auto comparisonData = ExtraDataBySubmeshIndex[i];

		Info.Description = comparisonData->Description;

		for (UInt32 j = 0; j < Header.nMaterial; ++j)
		{
			if (Materials[j].iSubMesh != i)
				continue;

			Info.Materials.push_back(&Materials[j]);
		}

		for (auto Material : Info.Materials)
		{
			for (UInt32 k = 0; k < Material->op_count; ++k)
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

void M2Lib::M2Skin::CopyMaterial(UInt32 SrcMeshIndex, UInt32 DstMeshIndex)
{
	auto Meshes = Elements[EElement_SubMesh].as<CElement_SubMesh>();
	auto Materials = Elements[EElement_Material].as<CElement_Material>();
	
	CElement_Material* SrcMaterial = NULL;
	for (UInt32 i = 0; i < Header.nMaterial; ++i)
	{
		if (Materials[i].iSubMesh != SrcMeshIndex)
			continue;

		SrcMaterial = &Materials[i];
	}

	if (!SrcMaterial)
		return;

	for (UInt32 i = 0; i < Header.nMaterial; ++i)
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

std::unordered_set<M2Lib::M2SkinElement::Edge> M2Lib::M2Skin::GetEdges(CElement_SubMesh* submesh)
{
	UInt16* Triangles = Elements[M2SkinElement::EElement_TriangleIndex].as<UInt16>();
	UInt16* Indices = Elements[M2SkinElement::EElement_VertexLookup].as<UInt16>();

	auto result = std::unordered_set<Edge>();
	std::unordered_map<UInt32, int> map;

	UInt32 TriangleIndexStart = submesh->GetStartTrianlgeIndex();
	UInt32 TriangleIndexEnd = submesh->GetEndTriangleIndex();
	for (UInt32 k = TriangleIndexStart; k < TriangleIndexEnd; k += 3)
	{
		assert(k + 2 < Elements[M2SkinElement::EElement_TriangleIndex].Count);

		UInt16 indexA = Indices[Triangles[k]];
		UInt16 indexB = Indices[Triangles[k + 1]];
		UInt16 indexC = Indices[Triangles[k + 2]];

		auto edgeA = Edge(indexA, indexB);
		auto edgeB = Edge(indexA, indexC);
		auto edgeC = Edge(indexC, indexB);

		auto itr = map.find(edgeA.GetHash());
		if (itr == map.end())
			map[edgeA.GetHash()] = 1;
		else
			++itr->second;

		itr = map.find(edgeB.GetHash());
		if (itr == map.end())
			map[edgeB.GetHash()] = 1;
		else
			++itr->second;

		itr = map.find(edgeC.GetHash());
		if (itr == map.end())
			map[edgeC.GetHash()] = 1;
		else
			++itr->second;
	}

	for (auto itr : map)
		if (itr.second == 1)
			result.insert(Edge((itr.first >> 16) & 0xFFFF, itr.first & 0xFFFF));

	return result;
}
