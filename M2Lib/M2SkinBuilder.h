#pragma once

#include "BaseTypes.h"
#include "M2Types.h"
#include "M2Skin.h"
#include "M2I.h"
#include <vector>
#include <map>
#include <assert.h>

namespace M2Lib
{
	class M2I;		// forward declaration.
	class M2;		// forward declaration.
	class M2Skin;	// forward declaration.

	// does the main heavy lifting of building new .skins for the M2.
	class M2SkinBuilder
	{
	public:
		//
		class CBonePartition
		{
		public:
			// maximum number of bones allowed per partition.
			uint32_t MaxBones;
			// list of bones in this partition, indices into the global bone list. later gets consolidated into the global bone lookup list.
			std::vector< uint16_t > Bones;
			// here we keep a map of triangle index to triangle for all triangles that have successfully been added to this bone partition. this is result caching to speed up building of subset partitions when dealing out subset triangles between subset partitions.
			std::map< uint32_t, CTriangle* > TrianglesMap;

			// offset from begining of skin's bone lookup list.
			uint32_t BoneStart;

		public:
			CBonePartition(uint32_t BoneLOD)
				: MaxBones(BoneLOD),
				BoneStart(0)
			{
			}

			// attemts to add all of the bones used by input triangle. returns true if bones already exist or were added and triangle was added. returns false if there is not enough room for additional bones.
			bool AddTriangle(CVertex* GlobalVertexList, CTriangle* pTriangle);

			// returns true if bone is contained in this bone partition. if pTriangleIndexOut is supplied and function returns true, it will be set to index of where bone was found in this partition.
			bool HasBone(uint16_t Bone, uint16_t* pIndexOut);

			// returns true if a triangle has been associated with this bone partition.
			bool HasTriangle(uint32_t TriangleIndex);
		};

		//
		class CSubMesh
		{
		public:
			class CSubsetPartition
			{
			public:
				// the bone partition used by this partitioned subset.
				CBonePartition* pBonePartition;

				// this subset partition's final index list. these are indices into the global vertex list. this gets consolidated into the single skin vertex list.
				//std::vector< uint16_t > Vertices;
				// this subset partition's final triangle list. these are indices into the above vertex index list. this gets consolidated into the single skin triangle listt
				std::vector< CTriangle* > Triangles;

				uint32_t VertexStart;
				uint32_t VertexCount;
				uint32_t TriangleIndexStart;
				uint32_t TriangleIndexCount;

				uint32_t Unknown1;	// aka max bones
				uint32_t Unknown2;	// aka category

			public:
				CSubsetPartition(CBonePartition* pBonePartitionIn);

				// attempts to add a triangle to this subset partition.
				bool AddTriangle(CTriangle* pTriangle);

				// adds a vertex from the global vertex list to this subset's vertex list. returns index of existing or newly added vertex.
				//uint32_t AddVertex( uint32_t VertexIndex );
				//
				//void FixVertexOffsets( int32_t Delta );

			};


		public:
			// this subset's ID within the model.
			uint16_t ID;
			//
			std::vector< CSubsetPartition* > SubsetPartitions;
			//
			//std::vector< M2Skin::CElement_Material* > pMaterials;

			SubmeshExtraData const* pExtraData;

		public:
			CSubMesh()
				: ID(0), pExtraData(NULL)
			{
			}

			~CSubMesh()
			{
				for (uint32_t i = 0; i < SubsetPartitions.size(); i++)
				{
					delete SubsetPartitions[i];
				}
			}

			// adds a subset partition to the list of subset partitions in this subset. this is done in preparation for when we deal out triangles and vertices to between the various subset partitions.
			void AddSubsetPartition(CBonePartition* pBonePartition);

			// attempts to add a triangle to this subset.
			bool AddTriangle(CTriangle* pTriangle);
		};

	public:
		// each skin has it's own vertex list. common vertices accross subsets get duplicated (i think) so they appear as many times as they are used in multiple subsets. this is because each subset occupies a sub-range of this list.
		std::vector< uint16_t > m_Vertices;
		// bone lookup list used by this skin. the bone lookup lists from all the skins get consolidated into one big bone lookup list that is stored in the M2.
		std::vector< uint16_t > m_Bones;
		// indices to m_VertexLookupList.
		std::vector< uint16_t > m_Indices;

		// list of subsets that make up this skin.
		std::vector< CSubMesh* > m_SubMeshList;

	public:
		M2SkinBuilder()
		{
		}

		~M2SkinBuilder()
		{
			for (uint32_t i = 0; i < m_SubMeshList.size(); i++)
			{
				delete m_SubMeshList[i];
			}
		}

	public:
		void Clear();

		// builds a skin from the supplied parameters.
		bool Build(M2Skin* pResult, uint32_t BoneLoD, M2I* pM2I, CVertex* pGlobalVertexList, uint32_t BoneStart);

		// returns true if the built skin with LoD is necessary to be exported, false if can be done without.
		// this is to check for LoD that has significant room for more bones than the skin actually uses, in such case, it would not be advisable to save.
	};
}
