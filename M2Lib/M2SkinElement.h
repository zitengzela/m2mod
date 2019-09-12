#pragma once

#include "BaseTypes.h"
#include "M2Types.h"
#include <map>
#include <unordered_set>
#include "VectorMath.h"

namespace M2Lib
{
	class M2Skin;

	enum SubsetType
	{
		Subset_Body = 0,
		Subset_Armor = 1,
		Subset_Cloak = 2,
		Subset_Hair = 3,
		Subset_Facial = 4,
		Subset_Unknown = 5
	};

	namespace M2SkinElement
	{
		typedef std::map<uint32_t, uint32_t> TextureLookupRemap;

		enum EElement
		{
			EElement_VertexLookup,
			EElement_TriangleIndex,
			EElement_BoneIndices,
			EElement_SubMesh,
			EElement_Material,
			EElement_Flags,

			EElement__Count__
		};

#pragma pack(push, 1)
		class CElement_VertexIndex
		{
		public:
			uint16_t GlobalVertexIndex;		// index into M2's global vertex list.
		};

		ASSERT_SIZE(CElement_VertexIndex, 2);

		class CElement_TriangleIndex
		{
		public:
			uint32_t VertexTriangleIndex;				// index into SKIN's vertex list.
		};

		ASSERT_SIZE(CElement_TriangleIndex, 4);

		class CElement_BoneIndices
		{
		public:
			union
			{
				uint32_t BoneIndicesPacked;
				uint8_t BoneIndices[BONES_PER_VERTEX];
			};

			void Clear() { BoneIndicesPacked = 0; }
		};

		ASSERT_SIZE(CElement_BoneIndices, 4);

		// each subset has at least one of these, defines a texture, shader, render flags, etc. basically a render state and texture stage/unit state to use when drawing the subset. multiple instances of these can be defined per subset, enabling multi-texturing.
		class CElement_Material
		{
			#define GLOSS_SHADER_ID 32769
			#define TRANSPARENT_SHADER_ID 16

		public:
			uint16_t Flags;						//
			uint16_t shader_id;					//
			uint16_t iSubMesh;					// the sub mesh index that this material is assigned to.
			uint16_t iSubMesh2;					// same as above, don't know why this is repeated.
			int16_t iColor;						// index into the color list.
			int16_t iRenderFlags;				//
			int16_t layer;						//
			int16_t op_count;					// shader to use for this texture stage.
			int16_t textureComboIndex;			// index into the texture lookup list.
			int16_t textureCoordComboIndex;		// index into the texture unit lookup list.
			int16_t textureWeightComboIndex;		// index into the transparency lookup list.
			int16_t textureTransformComboIndex;	// index into the texture lookup animation list.
		};

		ASSERT_SIZE(CElement_Material, 24);

		// actually, more like a subset partition, as multiple entries of these with same ID may exist.
		class CElement_SubMesh
		{
		public:
			// would map to Unkown2, but i don't think these are really what it means.
			enum ECategory
			{
				// these guesses taken from blood elf model
				// these values and usages seem to differ from model to model
				ECategory_FatBelt = 2,			// fat belt
				ECategory_Tabard = 3,			// tabard, shirt frills
				ECategory_LegUpper = 5,			// upper leg, skirt, armored leg, pant sleeve
				ECategory_Cape = 10,			// capes, and back peice that is used when no cape is equipped
				ECategory_LegLower = 11,		// leg lower, boots, pant frill
				ECategory_Body1 = 19,			// main body mesh, all hair styles, head plugs that close ear holes and bald hair style
				ECategory_Body2 = 27,			// main body mesh, all hair styles, head plugs that close ear holes and bald hair style
				ECategory_EyeGlow_Race = 32,
				ECategory_EyeLid = 35,
				ECategory_EyeGlow_DK = 36,
				ECategory_Features = 39,		// earings,
				ECategory_EyeBrow = 40,			//
				ECategory_ArmLower = 46,		// arm lower, wrist, gloves, sleeve frill
				ECategory_Ear1 = 51,			// ears, a couple earrings
				ECategory_Ear2 = 52,			// earrings
				ECategory_Ear3 = 53,			// more earrings
				ECategory_Ear4 = 57,			// more earrings
				ECategory_Ear5 = 59,			// more earrings
			};

		public:
			uint16_t ID;						// subset ID.
			uint16_t Level;					// subset level.
			uint16_t VertexStart;				// first vertex as index into skin's index list.
			uint16_t VertexCount;				// number of indices into the skin's index list.
			uint16_t TriangleIndexStart;		// first triangle as index into skin's triangle list, which if you want the real triangle index, divide by 3.
			uint16_t TriangleIndexCount;		// length of triangle list for this subset, which if you want the real triangle count, divide by 3.
			uint16_t BoneCount;				// number of bones to upload to GPU shader constant registers.
			uint16_t BoneStart;				// first bone in bone lookup list to begin upload to GPU shader constant registers.
			uint16_t MaxBonesPerVertex;		// always set from 0 to 4. maximum number of bones referenced by any one vertex in this sub mesh.
			uint16_t CenterBoneIndex;			// appers to be some sort of draw order sort index or z-depth bias value.
			C3Vector CenterMass;			// average position of all vertices in this subset. found by summing positions of all vertices and then dividing by the number of vertices.
			C3Vector SortCenter;			// bounding box center. if we make a minimum axis aligned bounding box around the set of vertices in this subset and get the center of that box, this is the result.
			float SortRadius;				// this is the distance of the vertex farthest from CenterBoundingBox.

			// FMN 2015-01-26: changing TriangleIndexstart, depending on ID. See http://forums.darknestfantasyerotica.com/showthread.php?20446-TUTORIAL-Here-is-how-WoD-.skin-works.&p=402561
			uint32_t GetStartTrianlgeIndex() const { return TriangleIndexStart + (uint32_t(Level) << 16); }
			uint32_t GetEndTriangleIndex() const { return GetStartTrianlgeIndex() + TriangleIndexCount; }
		};

		ASSERT_SIZE(CElement_SubMesh, 48);

		class CElement_Flags
		{
		public:
			uint16_t Flags1;			// set to 528 (common), 272 (common), 784 (uncommon), 912 (rare).
			uint16_t Unknown1;		// always 0.
			uint16_t iSubMesh;		// index of a sub mesh within this skin.
			uint16_t TextureId;		// 0 when Flags1 is set to 272, 1 when Flags 1 is set to 528, 4 when Flags1 is set to 784 or 912.
			int16_t ColorId;			// always set to 0 for first flags entry in file, always set to 65535/0xFFFF/-1 for subsequent entries after first.
			uint16_t TransparencyId;	// always set to 0.
		};

		ASSERT_SIZE(CElement_Flags, 12);
	}

	uint32_t GetSubSetType(uint32_t SubsetId);
	bool IsAlignedSubset(uint32_t SubsetId);

#pragma pack(pop)
}

namespace std
{
	template<>
	struct hash<M2Lib::Geometry::Edge>
	{
		std::size_t operator()(const M2Lib::Geometry::Edge& obj) const
		{
			uint32_t hash = (((uint32_t)obj.A->Index) << 16) | (uint32_t)obj.B->Index;

			return std::hash<uint32_t>()(hash);
		}
	};
}

