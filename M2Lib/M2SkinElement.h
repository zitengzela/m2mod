#pragma once

#include "BaseTypes.h"
#include "M2Types.h"
#include <map>
#include <unordered_set>

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
		typedef std::map<UInt32, UInt32> TextureLookupRemap;

		class Edge
		{
		public:
			Edge(UInt16 VertexA, UInt16 VertexB)
			{
				if (VertexA < VertexB)
				{
					A = VertexA;
					B = VertexB;
				}
				else
				{
					A = VertexB;
					B = VertexA;
				}
			}

			bool operator==(const Edge& other) const
			{
				return A == other.A && B == other.B;
			}

			UInt32 GetHash() const
			{
				return (UInt32(A) << 16) | UInt32(B);
			}

			UInt16 A;
			UInt16 B;
		};

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
			UInt16 GlobalVertexIndex;		// index into M2's global vertex list.
		};

		ASSERT_SIZE(CElement_VertexIndex, 2);

		class CElement_TriangleIndex
		{
		public:
			UInt32 VertexTriangleIndex;				// index into SKIN's vertex list.
		};

		ASSERT_SIZE(CElement_TriangleIndex, 4);

		class CElement_BoneIndices
		{
		public:
			union
			{
				UInt32 BoneIndicesPacked;
				UInt8 BoneIndices[BONES_PER_VERTEX];
			};
		};

		ASSERT_SIZE(CElement_BoneIndices, 4);

		// each subset has at least one of these, defines a texture, shader, render flags, etc. basically a render state and texture stage/unit state to use when drawing the subset. multiple instances of these can be defined per subset, enabling multi-texturing.
		class CElement_Material
		{
			#define GLOSS_SHADER_ID 32769
			#define TRANSPARENT_SHADER_ID 16

		public:
			UInt16 Flags;						//
			UInt16 shader_id;					//
			UInt16 iSubMesh;					// the sub mesh index that this material is assigned to.
			UInt16 iSubMesh2;					// same as above, don't know why this is repeated.
			SInt16 iColor;						// index into the color list.
			SInt16 iRenderFlags;				//
			SInt16 layer;						//
			SInt16 op_count;					// shader to use for this texture stage.
			SInt16 textureComboIndex;			// index into the texture lookup list.
			SInt16 textureCoordComboIndex;		// index into the texture unit lookup list.
			SInt16 textureWeightComboIndex;		// index into the transparency lookup list.
			SInt16 textureTransformComboIndex;	// index into the texture lookup animation list.
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
			UInt16 ID;						// subset ID.
			UInt16 Level;					// subset level.
			UInt16 VertexStart;				// first vertex as index into skin's index list.
			UInt16 VertexCount;				// number of indices into the skin's index list.
			UInt16 TriangleIndexStart;		// first triangle as index into skin's triangle list, which if you want the real triangle index, divide by 3.
			UInt16 TriangleIndexCount;		// length of triangle list for this subset, which if you want the real triangle count, divide by 3.
			UInt16 BoneCount;				// number of bones to upload to GPU shader constant registers.
			UInt16 BoneStart;				// first bone in bone lookup list to begin upload to GPU shader constant registers.
			UInt16 MaxBonesPerVertex;		// always set from 0 to 4. maximum number of bones referenced by any one vertex in this sub mesh.
			UInt16 CenterBoneIndex;			// appers to be some sort of draw order sort index or z-depth bias value.
			C3Vector CenterMass;			// average position of all vertices in this subset. found by summing positions of all vertices and then dividing by the number of vertices.
			C3Vector SortCenter;			// bounding box center. if we make a minimum axis aligned bounding box around the set of vertices in this subset and get the center of that box, this is the result.
			Float32 SortRadius;				// this is the distance of the vertex farthest from CenterBoundingBox.

			// FMN 2015-01-26: changing TriangleIndexstart, depending on ID. See http://forums.darknestfantasyerotica.com/showthread.php?20446-TUTORIAL-Here-is-how-WoD-.skin-works.&p=402561
			UInt32 GetStartTrianlgeIndex() const { return TriangleIndexStart + (UInt32(Level) << 16); }
			UInt32 GetEndTriangleIndex() const { return GetStartTrianlgeIndex() + TriangleIndexCount; }
		};

		ASSERT_SIZE(CElement_SubMesh, 48);

		class CElement_Flags
		{
		public:
			UInt16 Flags1;			// set to 528 (common), 272 (common), 784 (uncommon), 912 (rare).
			UInt16 Unknown1;		// always 0.
			UInt16 iSubMesh;		// index of a sub mesh within this skin.
			UInt16 TextureId;		// 0 when Flags1 is set to 272, 1 when Flags 1 is set to 528, 4 when Flags1 is set to 784 or 912.
			SInt16 ColorId;			// always set to 0 for first flags entry in file, always set to 65535/0xFFFF/-1 for subsequent entries after first.
			UInt16 TransparencyId;	// always set to 0.
		};

		ASSERT_SIZE(CElement_Flags, 12);
	}

	UInt32 GetSubSetType(UInt32 SubsetId);
	bool IsAlignedSubset(UInt32 SubsetId);

#pragma pack(pop)
}

namespace std
{
	template<>
	struct hash<M2Lib::M2SkinElement::Edge>
	{
		std::size_t operator()(const M2Lib::M2SkinElement::Edge& obj) const
		{
			uint32_t hash = (((UInt32)obj.A) << 16) | (UInt32)obj.B;

			return std::hash<uint32_t>()(hash);
		}
	};
}

