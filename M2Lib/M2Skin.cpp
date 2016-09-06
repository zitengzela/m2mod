#include "M2Skin.h"
#include "M2.h"
#include "M2.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <list>
#include <assert.h>

extern int g_Verbose;

M2Lib::EError M2Lib::M2Skin::Load(const Char16* FileName)
{
	// check path
	UInt32 Length = 0;
	while (FileName[Length] != '\0')
	{
		Length++;
	}
	if (Length >= 1024)
		return M2Lib::EError_PathTooLong;
	_FileName[Length] = '\0';
	for (UInt32 i = 0; i != Length; i++)
	{
		_FileName[i] = FileName[i];
	}

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::binary | std::ios::in);

	// find file size
	FileStream.seekg(0, std::ios::end);
	UInt32 FileSize = (UInt32)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	// load header
	FileStream.read((char*)&Header, sizeof(Header));

	// check header
	if (Header.ID[0] != 'S' || Header.ID[1] != 'K' || Header.ID[2] != 'I' || Header.ID[3] != 'N')
		return M2Lib::EError_FailedToLoadSKIN_FileMissingOrCorrupt;

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(FileSize);

	// load elements
	for (UInt32 i = 0; i != EElement__Count__; i++)
	{
		if (!Elements[i].Load(FileStream))
			return M2Lib::EError_FailedToLoadSKIN_FileMissingOrCorrupt;
	}

	// close file stream
	FileStream.close();

	//std::vector< CElement_SubMesh > SubMeshList;
	//for ( UInt32 i = 0; i < Elements[EElement_SubMesh].Count; i++ )
	//{
	//	SubMeshList.push_back( ((CElement_SubMesh*)Elements[EElement_SubMesh].Data)[i] );
	//}

	// resolve material lists
	for (UInt32 iSubMesh = 0; iSubMesh < Header.nSubMesh; iSubMesh++)
	{
		SubMeshList_MaterialList.push_back(std::vector< CElement_Material* >());
	}

	CElement_Material* MaterialArray = Elements[EElement_Material].as<CElement_Material>();
	for (UInt32 iMaterial = 0; iMaterial < Header.nMaterial; iMaterial++)
	{
		SubMeshList_MaterialList[MaterialArray[iMaterial].iSubMesh].push_back(&MaterialArray[iMaterial]);
	}

	// print info
	//PrintInfo();

	// done
	return M2Lib::EError_OK;
}


M2Lib::EError M2Lib::M2Skin::Save(const Char16* FileName)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::binary | std::ios::out | std::ios::trunc);

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// save header
	FileStream.write((Char8*)&Header, sizeof(Header));

	// save elements
	for (UInt32 i = 0; i != EElement__Count__; i++)
	{
		if (!Elements[i].Save(FileStream))
			return M2Lib::EError_FailedToSaveSKIN;
	}

	// close file stream
	FileStream.close();

	return M2Lib::EError_OK;
}


void M2Lib::M2Skin::BuildVertexBoneIndices()
{
	if (!pM2)
		return;

	CVertex* VertexList = pM2->Elements[M2::EElement_Vertex].as<CVertex>();

	UInt32 BoneLookupListLength = pM2->Elements[M2::EElement_SkinnedBoneLookup].Count;
	UInt16* BoneLookupList = pM2->Elements[M2::EElement_SkinnedBoneLookup].as<UInt16>();

	UInt32 VertexLookupListLength = Elements[EElement_VertexLookup].Count;
	UInt16* VertexLookupList = Elements[EElement_VertexLookup].as<UInt16>();

	Elements[EElement_BoneIndices].SetDataSize(VertexLookupListLength, VertexLookupListLength * sizeof(UInt32), false);
	UInt32* BoneIndexList = Elements[EElement_BoneIndices].as<UInt32>();

	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (UInt32 i = 0; i != VertexLookupListLength; i++)
	{
		BoneIndexList[i] = 0;
	}

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		UInt32 MaxBonesPerVertex = 0;

		std::vector< UInt16 > BonesLookupVector;
		for (SInt32 j = SubMesh.BoneStart; j < SubMesh.BoneStart + SubMesh.BoneCount; j++)
		{
			BonesLookupVector.push_back(BoneLookupList[j]);
		}

		UInt32 SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
		for (UInt32 j = SubMesh.VertexStart; j < SubMeshVertexEnd; j++)
		{
			CVertex const& Vertex = VertexList[VertexLookupList[j]];
			UInt8* BoneIndices8 = (UInt8*)&BoneIndexList[j];

			BoneIndices8[0] = m_ReverseBoneLookup(Vertex.BoneIndices[0], &BoneLookupList[SubMesh.BoneStart], SubMesh.BoneCount);
			BoneIndices8[1] = m_ReverseBoneLookup(Vertex.BoneIndices[1], &BoneLookupList[SubMesh.BoneStart], SubMesh.BoneCount);
			BoneIndices8[2] = m_ReverseBoneLookup(Vertex.BoneIndices[2], &BoneLookupList[SubMesh.BoneStart], SubMesh.BoneCount);
			BoneIndices8[3] = m_ReverseBoneLookup(Vertex.BoneIndices[3], &BoneLookupList[SubMesh.BoneStart], SubMesh.BoneCount);

			UInt32 MaxBonesPerThisVertex = 0;
			Vertex.BoneWeights[0] > 0 ? MaxBonesPerThisVertex++ : 0;
			Vertex.BoneWeights[1] > 0 ? MaxBonesPerThisVertex++ : 0;
			Vertex.BoneWeights[2] > 0 ? MaxBonesPerThisVertex++ : 0;
			Vertex.BoneWeights[3] > 0 ? MaxBonesPerThisVertex++ : 0;

			if (MaxBonesPerThisVertex > MaxBonesPerVertex) MaxBonesPerVertex = MaxBonesPerThisVertex;
		}

		SubMesh.MaxBonesPerVertex = MaxBonesPerVertex;
	}
}

void M2Lib::M2Skin::BuildBoundingData()
{
	CVertex* VertexList = pM2->Elements[M2::EElement_Vertex].as<CVertex>();
	UInt16* VertexLookupList = Elements[EElement_VertexLookup].as<UInt16>();

	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		if (SubMesh.VertexCount)
		{
			std::vector<M2Lib::CVertex> vertices;
			UInt32 SubMeshVertexEnd = SubMesh.VertexStart + SubMesh.VertexCount;
			for (UInt32 j = SubMesh.VertexStart; j < SubMeshVertexEnd; j++)
			{
				vertices.push_back(VertexList[VertexLookupList[j]]);
			}

			BoundaryData boundary;
			boundary.Calculate(vertices);

			SubMesh.CenterMass[0] = boundary.CenterMass.X;
			SubMesh.CenterMass[1] = boundary.CenterMass.Y;
			SubMesh.CenterMass[2] = boundary.CenterMass.Z;

			SubMesh.CenterBounds[0] = boundary.BoundingCenter.X;
			SubMesh.CenterBounds[1] = boundary.BoundingCenter.Y;
			SubMesh.CenterBounds[2] = boundary.BoundingCenter.Z;

			SubMesh.Radius = boundary.BoundingRadius;
		}
	}
}

void M2Lib::M2Skin::BuildMaxBones()
{
	CVertex* VertexList = pM2->Elements[M2::EElement_Vertex].as<CVertex>();

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

	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		CElement_SubMesh& SubMesh = SubMeshList[iSubMesh];

		auto comparisonDataItr = ComparisonDataBySubmeshIndex.find(iSubMesh);
		assert(comparisonDataItr != ComparisonDataBySubmeshIndex.end());

		SInt32 SubMeshOtherTriangleIndex;
		CElement_SubMesh* SubMeshOther = pOther->GetSubMesh(*comparisonDataItr->second, SubMeshOtherTriangleIndex);
		assert(SubMeshOther);

		SubMesh.SortTriangleIndex = SubMeshOther->SortTriangleIndex;
		// copy level from original mesh
		SubMesh.Level = SubMeshOther->Level;

		std::vector< CElement_Material* > SubMeshOtherMaterialList;
		pOther->GetSubMeshMaterials(SubMeshOtherTriangleIndex, SubMeshOtherMaterialList);
		for (UInt32 iSubMeshMaterialOther = 0; iSubMeshMaterialOther < SubMeshOtherMaterialList.size(); iSubMeshMaterialOther++)
		{
			CElement_Material NewMaterial = *SubMeshOtherMaterialList[iSubMeshMaterialOther];
			NewMaterial.iSubMesh = iSubMesh;
			NewMaterial.iSubMesh2 = iSubMesh;
			/*for (int i = 0; i < SubMeshOtherMaterialList.size(); ++i)
			{
				if (SubMeshOtherMaterialList[i]->iSubMesh == iSubMesh)
				NewMaterial.iSubMesh2 = SubMeshOtherMaterialList[i]->iSubMesh2;
			}*/
			NewMaterialList.push_back(NewMaterial);
		}

		std::vector< CElement_Flags* > SubMeshOtherFlagsList;
		pOther->GetSubMeshFlags(SubMeshOtherTriangleIndex, SubMeshOtherFlagsList);
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

bool SortSubMeshesComparisonFunction(M2Lib::M2Skin::CElement_SubMesh* A, M2Lib::M2Skin::CElement_SubMesh* B)
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
	std::list< M2Lib::M2Skin::CElement_SubMesh* > SubMeshListSorted;
	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		SubMeshListSorted.push_back(&SubMeshList[iSubMesh]);
	}
	SubMeshListSorted.sort(SortSubMeshesComparisonFunction);

	// build index remap list
	UInt32* SubMeshReMapList = new UInt32[SubMeshListLength];
	for (UInt32 i = 0; i < SubMeshListLength; i++)
	{
		std::list< M2Lib::M2Skin::CElement_SubMesh* >::iterator it = SubMeshListSorted.begin();
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
	M2Lib::M2Skin::CElement_SubMesh* NewSubMeshList = new M2Lib::M2Skin::CElement_SubMesh[SubMeshListLength];
	std::list< M2Lib::M2Skin::CElement_SubMesh* >::iterator SubMeshListSortedIterator = SubMeshListSorted.begin();
	for (UInt32 iSubMesh = 0; iSubMesh < SubMeshListLength; iSubMesh++)
	{
		NewSubMeshList[iSubMesh] = *(*SubMeshListSortedIterator);
		SubMeshListSortedIterator++;
	}
	memcpy(Elements[EElement_SubMesh].Data.data(), NewSubMeshList, Elements[EElement_SubMesh].Data.size());
	delete[] NewSubMeshList;
}


M2Lib::M2Skin::CElement_SubMesh* M2Lib::M2Skin::GetSubMesh(SubmeshComparisonData const& TargetSubMeshData, SInt32& SubMeshTriangleIndexOut)
{
	UInt32 SubMeshListLength = Elements[EElement_SubMesh].Count;
	CElement_SubMesh* SubMeshList = Elements[EElement_SubMesh].as<CElement_SubMesh>();

	Float32 DeltaMin = 0.0f;
	SInt32 ClosestMatch = -1;
	SubMeshTriangleIndexOut = -1;

	UInt16 ID = TargetSubMeshData.ID;
	C3Vector const& CenterBounds = TargetSubMeshData.Boundary.BoundingCenter;
	C3Vector const& CenterMass = TargetSubMeshData.Boundary.CenterMass;

	for (UInt32 i = 0; i != SubMeshListLength; i++)
	{
		if (SubMeshList[i].ID == ID)
		{
			Float32 X = 0.0f;
			Float32 Y = 0.0f;
			Float32 Z = 0.0f;
			Float32 Delta = 0.0f;

			X = SubMeshList[i].CenterBounds[0] - CenterBounds.X;
			Y = SubMeshList[i].CenterBounds[1] - CenterBounds.Y;
			Z = SubMeshList[i].CenterBounds[2] - CenterBounds.Z;
			Delta = (X * X) + (Y * Y) + (Z * Z);

			X = SubMeshList[i].CenterMass[0] - CenterMass.X;
			Y = SubMeshList[i].CenterMass[1] - CenterMass.Y;
			Z = SubMeshList[i].CenterMass[2] - CenterMass.Z;
			Delta += (X * X) + (Y * Y) + (Z * Z);

			if (ClosestMatch == -1 || Delta < DeltaMin)
			{
				DeltaMin = Delta;
				ClosestMatch = i;
			}
		}
	}

	if (ClosestMatch >= 0)
	{
		SubMeshTriangleIndexOut = ClosestMatch;
		return &SubMeshList[ClosestMatch];
	}

	return 0;
}


void M2Lib::M2Skin::GetSubMeshMaterials(UInt32 SubMeshTriangleIndex, std::vector< CElement_Material* >& Result)
{
	CElement_Material* MaterialList = Elements[EElement_Material].as<CElement_Material>();
	for (UInt32 iMaterial = 0; iMaterial < Header.nMaterial; iMaterial++)
	{
		if (MaterialList[iMaterial].iSubMesh == SubMeshTriangleIndex)
		{
			Result.push_back(&MaterialList[iMaterial]);
		}
	}
}


void M2Lib::M2Skin::GetSubMeshFlags(UInt32 SubMeshTriangleIndex, std::vector< CElement_Flags* >& Result)
{
	CElement_Flags* FlagsList = Elements[EElement_Flags].as<CElement_Flags>();
	for (UInt32 iFlags = 0; iFlags < Header.nFlags; iFlags++)
	{
		if (FlagsList[iFlags].iSubMesh == SubMeshTriangleIndex)
		{
			Result.push_back(&FlagsList[iFlags]);
		}
	}
}


bool M2Lib::M2Skin::PrintInfo()
{
	UInt32 Count = 0;

	Char16 szFileOut[1024];
	UInt32 Length = 0;
	while (_FileName[Length] != '\0')
	{
		szFileOut[Length] = _FileName[Length];
		Length++;
	}
	szFileOut[Length++] = '.';
	szFileOut[Length++] = 't';
	szFileOut[Length++] = 'x';
	szFileOut[Length++] = 't';
	szFileOut[Length++] = '\0';

	std::fstream FileStream;
	FileStream.open(szFileOut, std::ios::out | std::ios::trunc);

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

	FileStream << "Unknown1:      " << Header.Unknown1 << std::endl;
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
		FileStream << "SortTriangleIndex:     " << Subset.SortTriangleIndex << std::endl;
		FileStream << "CenterMass:   " << "( " << Subset.CenterMass[0] << ", " << Subset.CenterMass[1] << ", " << Subset.CenterMass[2] << " ) " << std::endl;
		FileStream << "CenterBounds: " << "( " << Subset.CenterBounds[0] << ", " << Subset.CenterBounds[1] << ", " << Subset.CenterBounds[2] << " ) " << std::endl;
		FileStream << "Radius:       " << Subset.Radius << std::endl;
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
		FileStream << "Flags2:     " << Flags.Flags2 << std::endl;
		FileStream << "Unknown2:   " << Flags.Unknown2 << std::endl;
		FileStream << "Unknown3:   " << Flags.Unknown3 << std::endl;
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
        FileStream << "iTexture:   " << Material.iTexture << std::endl;
        FileStream << "iTexutreUnit2:   " << Material.iTexutreUnit2 << std::endl;

        FileStream << "iTransparency:   " << Material.iTransparency << std::endl;
        FileStream << "iTextureAnimation:   " << Material.iTextureAnimation << std::endl;
        FileStream << std::endl;
    }


    // CElement_Material

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

	Elements[EElement_Flags].Count = Header.nFlags;
	Elements[EElement_Flags].Offset = Header.oFlags;
}


void M2Lib::M2Skin::m_LoadElements_FindSizes(UInt32 FileSize)
{
	for (UInt32 i = 0; i != EElement__Count__; i++)
	{
		Elements[i].OffsetOriginal = Elements[i].Offset;

		if ((Elements[i].Count == 0) || (Elements[i].Offset == 0))
		{
			Elements[i].Data.clear();
		}
		else
		{
			UInt32 NextOffset = FileSize;
			for (UInt32 j = i + 1; j < EElement__Count__; j++)
			{
				if (Elements[j].Offset)
				{
					NextOffset = Elements[j].Offset;
					break;
				}
			}
			Elements[i].Data.resize(NextOffset - Elements[i].Offset);
		}
	}
}


void M2Lib::M2Skin::m_SaveElements_FindOffsets()
{
	UInt32 CurrentOffset = sizeof(Header);
	CurrentOffset = (CurrentOffset + 15) & ~15;

	for (UInt32 i = 0; i < EElement__Count__; i++)
	{
		if (!Elements[i].Data.empty())
		{
			Elements[i].Offset = CurrentOffset;
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

	Header.Unknown1 = 0;

	Header.nFlags = Elements[EElement_Flags].Count;
	Header.oFlags = Elements[EElement_Flags].Offset;

	Header.Unknown2 = 0;
	Header.Unknown3 = 0;
}
