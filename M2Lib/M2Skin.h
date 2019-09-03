#pragma once

#include "DataElement.h"
#include "M2SkinElement.h"
#include "M2Types.h"
#include <vector>
#include <map>

namespace M2Lib
{
	class M2;

	namespace M2Element
	{
		class CElement_Texture;
	}

	class M2Skin
	{
		friend class M2;

	public:

#pragma pack(push,1)
		// memory mapped header
		class CM2SkinHeader
		{
		public:
			char ID[4];	// 'SKIN'

			uint32_t nVertex;
			uint32_t oVertex;

			uint32_t nTriangleIndex;
			uint32_t oTriangleIndex;

			uint32_t nBoneIndices;
			uint32_t oBoneIndices;

			uint32_t nSubMesh;
			uint32_t oSubMesh;

			uint32_t nMaterial;
			uint32_t oMaterial;

			uint32_t BoneCountMax;	// non-zero only in ChickenMount01.skin

			uint32_t nFlags;
			uint32_t oFlags;

			uint32_t Unknown2;	// non-zero only in ChickenMount01.skin
			uint32_t Unknown3;	// non-zero only in ChickenMount01.skin
		};

#pragma pack(pop)

	public:
		M2Skin(M2* pM2In)
			: pM2(pM2In)
		{
		}

		~M2Skin()
		{
		}


	public:
		std::wstring _FileName;

		CM2SkinHeader Header;
		DataElement Elements[M2SkinElement::EElement__Count__];

		std::vector<SubmeshExtraData const*> ExtraDataBySubmeshIndex;

		// pointer to M2 that this skin belongs to.
		M2* pM2;

	public:
		// loads an M2 skin from a file.
		EError Load(const wchar_t* FileName);
		// saves this M2 skin to a file.
		EError Save(const wchar_t* FileName);

		void BuildVertexBoneIndices();
		void BuildBoundingData();
		void BuildMaxBones();

		// copies materials from sub meshes in another skin to equivalent sub meshes in this skin.
		void CopyMaterials(M2Skin* pOther);

		//
		void SortSubMeshes();

		bool PrintInfo();

		// returns sub mesh with ID using CenterBounds to narrow search. some times there are multiple sub meshes with the same ID, so we can narrow our search to whatever sub mesh lies closest to CenterBounds.
		M2SkinElement::CElement_SubMesh* GetSubMesh(SubmeshExtraData const& TargetSubMeshData, int32_t& SubMeshIndexOut);

		// gathers list of materials that affect a given sub mesh within this skin.
		void GetSubMeshMaterials(uint32_t SubMeshIndex, std::vector< M2SkinElement::CElement_Material* >& Result);

		// gathers list of flags that affect a given sub mesh within this skin.
		void GetSubMeshFlags(uint32_t SubMeshIndex, std::vector< M2SkinElement::CElement_Flags* >& Result);

		bool AddShader(uint16_t ShaderId, int16_t const* TextureTypes, std::string const* MeshTextures, std::vector<uint32_t> const& MeshIndexes, M2SkinElement::TextureLookupRemap& LookupRemap);
		bool AddShader(uint16_t ShaderId, uint32_t const* MeshTextureIds, std::vector<uint32_t> const& MeshIndexes, M2SkinElement::TextureLookupRemap& LookupRemap);

		void CopyMaterial(uint32_t SrcMeshIndex, uint32_t DstMeshIndex);

		std::unordered_set<M2SkinElement::Edge> GetEdges(M2SkinElement::CElement_SubMesh* submesh);

		class MeshInfo
		{
		public:
			MeshInfo() : ID(0), pSubMesh(NULL)
			{
			}

			struct TextureInfo
			{
				M2Element::CElement_Texture* pTexture;
				std::string Name;
			};

			uint32_t ID;
			std::string Description;

			std::vector<M2SkinElement::CElement_Material*> Materials;
			std::vector<TextureInfo> Textures;
			M2SkinElement::CElement_SubMesh* pSubMesh;
		};
		std::vector<MeshInfo> GetMeshInfo();

	private:
		void m_LoadElements_CopyHeaderToElements();
		void m_LoadElements_FindSizes(uint32_t FileSize);
		void m_SaveElements_FindOffsets();
		void m_SaveElements_CopyElementsToHeader();

		static int32_t m_ReverseBoneLookup(uint8_t BoneID, uint16_t* BoneLookupTable, uint32_t BoneLookupTableLength)
		{
			for (uint32_t i = 0; i < BoneLookupTableLength; i++)
			{
				if (BoneLookupTable[i] == BoneID)
					return i;
			}

			return -1;
		}
	};
}
