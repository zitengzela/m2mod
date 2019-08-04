#include "M2SkinBuilder.h"
#include "M2I.h"
#include "M2.h"
#include "M2Skin.h"
#include "Logger.h"
#include <cstring>
#include <iostream>

bool M2Lib::M2SkinBuilder::CBonePartition::AddTriangle(CVertex* GlobalVertexList, CTriangle* pTriangle)
{
	// put all the bones used by the input triangle into a 1D list for easy iteration
	int16_t TriBones[BONES_PER_TRIANGLE];

	for (int i = 0; i < VERTEX_PER_TRIANGLE; ++i)
	{
		uint32_t TotalWeight = 0;
		int16_t* TriBonesSub = &TriBones[i * BONES_PER_VERTEX];
		CVertex* pTriVertex = &GlobalVertexList[pTriangle->Vertices[i]];

		for (int j = 0; j < BONES_PER_VERTEX; ++j)
		{
			TriBonesSub[j] = pTriVertex->BoneWeights[j] ? pTriVertex->BoneIndices[j] : -1;
			TotalWeight += pTriVertex->BoneWeights[j];
		}

		//assert(TotalWeight == 255);
	}

	// count the number of bones used by the triangle that don't exist in partition
	uint32_t ExtraBones = 0;
	for (uint32_t i = 0; i < BONES_PER_TRIANGLE; ++i)
	{
		if (TriBones[i] != -1 && !HasBone(TriBones[i], NULL))
			++ExtraBones;
	}

	if (ExtraBones > 0)
	{
		// there are some bones from the input triangle that are not contained in this bone partition
		if (ExtraBones + Bones.size() > MaxBones)
		{
			// there isn't enough room for them
			return false;
		}

		// there's room for them
		for (uint32_t i = 0; i < BONES_PER_TRIANGLE; ++i)
		{
			if (TriBones[i] != -1 && !HasBone(TriBones[i], NULL))
			{
				// add the bone that isn't already contained
				Bones.push_back(TriBones[i]);
			}
		}
	}

	// add triangle index and triangle to the triangle map
	assert(!HasTriangle(pTriangle->TriangleIndex));
	TrianglesMap[pTriangle->TriangleIndex] = pTriangle;

	// triangle successfully added
	return true;
}

bool M2Lib::M2SkinBuilder::CBonePartition::HasBone(uint16_t BoneIndex, uint16_t* pIndexOut)
{
	for (uint32_t i = 0; i < Bones.size(); ++i)
	{
		if (Bones[i] == BoneIndex)
		{
			if (pIndexOut)
				*pIndexOut = i;
			return true;
		}
	}

	return false;
}

bool M2Lib::M2SkinBuilder::CBonePartition::HasTriangle(uint32_t TriangleIndex)
{
	return TrianglesMap.find(TriangleIndex) != TrianglesMap.end();
}


M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::CSubsetPartition(CBonePartition* pBonePartitionIn)
{
	pBonePartition = pBonePartitionIn;

	VertexStart = 0;
	VertexCount = 0;
	TriangleIndexStart = 0;
	TriangleIndexCount = 0;

	Unknown1 = 0;
	Unknown2 = 0;
}

bool M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::AddTriangle(CTriangle* pTriangle)
{
	if (pBonePartition->HasTriangle(pTriangle->TriangleIndex))
	{
		Triangles.push_back(pTriangle);

		return true;
	}

	return false;
}


//uint32_t M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::AddVertex( uint32_t VertexTriangleIndex )
//{
//	uint32_t Count = Vertices.size();
//	for ( uint32_t i = 0; i < Count; i++ )
//	{
//		if ( Vertices[i] == VertexTriangleIndex )
//		{
//			return i;
//		}
//	}
//
//	Vertices.push_back( VertexTriangleIndex );
//	return Count;
//}

//void M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::FixVertexOffsets( int32_t Delta )
//{
//	for ( uint32_t i = 0; i < Triangles.size(); i++ )
//	{
//		CTriangle* pTriangle = &Triangles[i];
//
//		pTriangle->Vertices[0] += Delta;
//		pTriangle->Vertices[1] += Delta;
//		pTriangle->Vertices[2] += Delta;
//	}
//}

void M2Lib::M2SkinBuilder::CSubMesh::AddSubsetPartition(CBonePartition* pBonePartition)
{
	SubsetPartitions.push_back(new CSubsetPartition(pBonePartition));
}

bool M2Lib::M2SkinBuilder::CSubMesh::AddTriangle(CTriangle* pTriangle)
{
	for (uint32_t i = 0; i < SubsetPartitions.size(); i++)
	{
		if (SubsetPartitions[i]->AddTriangle(pTriangle))
			return true;
	}

	return false;
}

void M2Lib::M2SkinBuilder::Clear()
{
	m_Vertices.clear();
	m_Bones.clear();
	m_Indices.clear();

	for (uint32_t i = 0; i < m_SubMeshList.size(); i++)
	{
		delete m_SubMeshList[i];
	}
	m_SubMeshList.clear();
}

bool M2Lib::M2SkinBuilder::Build(M2Skin* pResult, uint32_t BoneLoD, M2I* pM2I, CVertex* pGlobalVertexList, uint32_t BoneStart)
{
	Clear();

	// list of bone partitions used within this skin.
	std::vector<CBonePartition*> m_BonePartitions;

	for (uint32_t i = 0; i < pM2I->SubMeshList.size(); ++i)
	{
		auto SubMesh = pM2I->SubMeshList[i];

		for (uint32_t j = 0; j < SubMesh->Triangles.size(); ++j)
		{
			bool Added = false;
			for (uint32_t k = 0; k < m_BonePartitions.size(); ++k)
			{
				if (m_BonePartitions[k]->AddTriangle(pGlobalVertexList, &SubMesh->Triangles[j]))
				{
					Added = true;
					break;
				}
			}

			if (!Added)
			{
				auto partition = new CBonePartition(BoneLoD);
				assert(partition->AddTriangle(pGlobalVertexList, &SubMesh->Triangles[j]));
				m_BonePartitions.push_back(partition);
			}
		}
	}

	uint32_t iBoneStart = BoneStart;
	for (uint32_t i = 0; i < m_BonePartitions.size(); ++i)
	{
		m_BonePartitions[i]->BoneStart = iBoneStart;
		iBoneStart += m_BonePartitions[i]->Bones.size();
	}

	for (uint32_t i = 0; i < pM2I->SubMeshList.size(); ++i)
	{
		CSubMesh* pNewSubset = new CSubMesh();
		pNewSubset->ID = pM2I->SubMeshList[i]->ID;
		pNewSubset->pExtraData = &pM2I->SubMeshList[i]->ExtraData;

		// add sub mesh partitions
		for (uint32_t k = 0; k < m_BonePartitions.size(); k++)
			pNewSubset->AddSubsetPartition(m_BonePartitions[k]);

		m_SubMeshList.push_back(pNewSubset);
	}

	for (uint32_t i = 0; i < m_SubMeshList.size(); ++i)
	{
		M2I::CSubMesh* pSubMeshM2I = pM2I->SubMeshList[i];
		for (uint32_t j = 0; j < pSubMeshM2I->Triangles.size(); ++j)
		{
			assert(m_SubMeshList[i]->AddTriangle(&pSubMeshM2I->Triangles[j]));
		}
	}

	uint32_t VertexStart = 0;
	uint32_t TriangleIndexStart = 0;
	for (uint32_t i = 0; i < m_SubMeshList.size(); ++i)
	{
		for (uint32_t j = 0; j < m_SubMeshList[i]->SubsetPartitions.size(); ++j)
		{
			CSubMesh::CSubsetPartition* pSubsetPartition = m_SubMeshList[i]->SubsetPartitions[j];
			if (pSubsetPartition->Triangles.empty())
				continue;

			std::map< uint16_t, uint16_t > GlobalToSkinIndexMap;
			uint32_t VertexCount = 0;
			uint32_t TriangleIndexCount = 0;
			for (uint32_t k = 0; k < pSubsetPartition->Triangles.size(); ++k)
			{
				for (uint32_t iVert = 0; iVert < VERTEX_PER_TRIANGLE; ++iVert)
				{
					uint16_t VertexToMap = pSubsetPartition->Triangles[k]->Vertices[iVert];	// this is the global vertex index
					uint16_t VertexMapped = 0;
					if (GlobalToSkinIndexMap.find(VertexToMap) != GlobalToSkinIndexMap.end())
						VertexMapped = GlobalToSkinIndexMap[VertexToMap];
					else
					{
						VertexMapped = m_Vertices.size();
						m_Vertices.push_back(VertexToMap);
						GlobalToSkinIndexMap[VertexToMap] = VertexMapped;
						++VertexCount;
					}
					m_Indices.push_back(VertexMapped);
					++TriangleIndexCount;
				}
			}
			pSubsetPartition->VertexStart = VertexStart;
			pSubsetPartition->VertexCount = VertexCount;
			VertexStart += VertexCount;
			pSubsetPartition->TriangleIndexStart = TriangleIndexStart;
			pSubsetPartition->TriangleIndexCount = TriangleIndexCount;
			TriangleIndexStart += TriangleIndexCount;
		}
	}

	for (auto& BonePartition : m_BonePartitions)
	{
		for (auto Bone : BonePartition->Bones)
			m_Bones.push_back(Bone);
	}

	// fill result
	pResult->Header.ID[0] = 'S';
	pResult->Header.ID[1] = 'K';
	pResult->Header.ID[2] = 'I';
	pResult->Header.ID[3] = 'N';

	//pResult->Header.LevelOfDetail = m_MaxBones;
	pResult->Header.BoneCountMax = 0;
	pResult->Header.Unknown2 = 0;
	pResult->Header.Unknown3 = 0;

	pResult->Elements[M2SkinElement::EElement_VertexLookup].SetDataSize(m_Vertices.size(), m_Vertices.size() * sizeof(uint16_t), false);
	uint16_t* Indices = pResult->Elements[M2SkinElement::EElement_VertexLookup].as<uint16_t>();
	memcpy(Indices, m_Vertices.data(), m_Vertices.size() * sizeof(uint16_t));

	pResult->Elements[M2SkinElement::EElement_TriangleIndex].SetDataSize(m_Indices.size(), m_Indices.size() * sizeof(uint16_t), false);
	uint16_t* Triangles = pResult->Elements[M2SkinElement::EElement_TriangleIndex].as<uint16_t>();
	memcpy(Triangles, m_Indices.data(), m_Indices.size() * sizeof(uint16_t)); // triangles

	uint32_t TotalPartitionCount = 0;
	for (uint32_t i = 0; i < m_SubMeshList.size(); ++i)
		for (uint32_t j = 0; j < m_SubMeshList[i]->SubsetPartitions.size(); ++j)
			if (m_SubMeshList[i]->SubsetPartitions[j]->Triangles.size())
				++TotalPartitionCount;

	pResult->Elements[M2SkinElement::EElement_SubMesh].SetDataSize(TotalPartitionCount, TotalPartitionCount * sizeof(M2SkinElement::CElement_SubMesh), false);
	M2SkinElement::CElement_SubMesh* SubsetsOut = pResult->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	uint32_t iSubsetPartition = 0;

	for (uint32_t i = 0; i < m_SubMeshList.size(); i++)
	{
		for (uint32_t j = 0; j < m_SubMeshList[i]->SubsetPartitions.size(); ++j)
		{
			CSubMesh::CSubsetPartition* pSubsetPartitionIn = m_SubMeshList[i]->SubsetPartitions[j];
			if (!pSubsetPartitionIn->Triangles.size())
				continue;

			M2SkinElement::CElement_SubMesh* pSubsetOut = &SubsetsOut[iSubsetPartition];

			pSubsetOut->ID = m_SubMeshList[i]->ID;
			pSubsetOut->Level = pSubsetPartitionIn->TriangleIndexStart >> 16;
			pSubsetOut->VertexStart = pSubsetPartitionIn->VertexStart;
			pSubsetOut->VertexCount = pSubsetPartitionIn->VertexCount;
			pSubsetOut->TriangleIndexStart = pSubsetPartitionIn->TriangleIndexStart & 0xFFFF;
			pSubsetOut->TriangleIndexCount = pSubsetPartitionIn->TriangleIndexCount;
			pSubsetOut->BoneStart = pSubsetPartitionIn->pBonePartition->BoneStart;
			pSubsetOut->BoneCount = pSubsetPartitionIn->pBonePartition->Bones.size();

			// we do the real calculation for this later
			pSubsetOut->MaxBonesPerVertex = 0;

			// don't know what this is
			//pSubsetOut->Unknown2 = pSubsetPartitionIn->Unknown2;

			// store comparison data that is calculated from original mesh before it was separated
			pResult->ExtraDataBySubmeshIndex.push_back(m_SubMeshList[i]->pExtraData);

			++iSubsetPartition;
		}
	}

	pResult->BuildBoundingData();

	return true;
}

