#pragma once

#include "BaseTypes.h"
#include "M2Types.h"

namespace M2Lib
{
	namespace M2Element
	{
		// named indices.
		enum EElement
		{
			EElement_Name = 0,
			EElement_GlobalSequence = 1,
			EElement_Animation = 2,
			EElement_AnimationLookup = 3,
			EElement_Bone = 4,
			EElement_KeyBoneLookup = 5,
			EElement_Vertex = 6,
			EElement_Color = 7,
			EElement_Texture = 8,
			EElement_Transparency = 9,
			EElement_TextureAnimation = 10,
			EElement_TextureReplace = 11,
			EElement_TextureFlags = 12,
			EElement_SkinnedBoneLookup = 13,
			EElement_TextureLookup = 14,
			EElement_TextureUnitLookup = 15,
			EElement_TransparencyLookup = 16,
			EElement_TextureAnimationLookup = 17,
			EElement_BoundingTriangle = 18,
			EElement_BoundingVertex = 19,
			EElement_BoundingNormal = 20,
			EElement_Attachment = 21,
			EElement_AttachmentLookup = 22,
			EElement_Event = 23,
			EElement_Light = 24,
			EElement_Camera = 25,
			EElement_CameraLookup = 26,
			EElement_RibbonEmitter = 27,
			EElement_ParticleEmitter = 28,
			EElement_TextureCombinerCombo = 29,

			EElement__Count__
		};

#pragma pack(push, 1)
		//
		//
		class CElement_Name
		{
		public:
			char* szName;

		public:
			CElement_Name()
			{
				szName = 0;
			}

			~CElement_Name()
			{
				if (szName)
				{
					delete[] szName;
				}
			}
		};


		//
		//
		class CElement_GlobalSequence
		{
		public:
			uint32_t Value;
		};

		class CElement_AnimationLookup
		{
		public:
			uint16_t AnimationID;
		};

		ASSERT_SIZE(CElement_GlobalSequence, 4);

		//
		// some sort of animation entry (without any apparent referenced animation data).
		class CElement_Animation
		{
		public:
			uint16_t AnimationID;
			uint16_t AnimationSubID;
			uint32_t Length;

			float MoveSpeed;

			uint32_t Flags;

			int16_t Rarity;

			uint16_t Padding;
			uint32_t BlendTimeIn;
			uint32_t BlendTimeOut;

			uint32_t PlaySpeed;

			SVolume BoundingVolume;

			int16_t NextAnimation;		// index to next animation with same AnimationID, -1 if there are no more.
			uint16_t NextIndex;			// this animation's index in the list of animations.

			bool IsInplace() const { return (Flags & 0x20) != 0; }
		};

		ASSERT_SIZE(CElement_Animation, 64);

		enum EInterpolationType
			: uint16_t
		{
			EInterpolationType_None = 0,
			EInterpolationType_Linear = 1,
			EInterpolationType_Bezier = 2,
			EInterpolationType_Hermite = 3,
		};

		struct M2TrackBase
		{
			EInterpolationType InterpolationType;
			int16_t GlobalSequenceID;
			M2Array TimeStamps;
		};

		ASSERT_SIZE(M2TrackBase, 0xC);

		//
		// generic animation block header.
		struct M2Track : M2TrackBase
		{
			// an animation will reference several of these, and each of these in turn corresponds to a bone (my best guess).
			class CChannel
			{
			public:
				uint32_t n;	// number
				uint32_t o;	// offset
			};

			// used for times?
			struct SKey_UInt32
			{
				uint32_t Values[1];
			};

			// used for position and scale keys.
			struct SKey_Float32x3
			{
				float Values[3];
			};

			// used for rotation quaternion keys.
			struct SKey_SInt16x4
			{
				uint16_t Values[4];
			};

			// used for visibility, probably interpreted as boolean.
			struct SKey_UInt16
			{
				uint16_t Values[1];
			};

			// used for light intensity, etc.
			struct SKey_Float32
			{
				float Values[1];
			};

			M2Array Values;
		};

		ASSERT_SIZE(M2Track, 20);

		class CElement_FakeAnimationBlock
		{
		public:
			M2Array Times;
			M2Array Keys;
		};

		ASSERT_SIZE(CElement_FakeAnimationBlock, 16);

		//
		// a list of these makes up a skeleton.
		class CElement_Bone
		{
		public:
			enum EFlags
				: uint32_t
			{
				EFlags_SphericalBillboard = 0x8,
				EFlags_BilboardLockX = 0x10,
				EFlags_BilboardLockY = 0x20,
				EFlags_BilboardLockZ = 0x40,
				EFlags_Transformed = 0x200,
				EFlags_Kinematic = 0x400,		// MoP+: allow physics to influence this bone
				EFlags_AnimScaled = 0x1000,		// set blend_modificator to helmetAnimScalingRec.m_amount for this bone
			};

			int32_t BoneLookupID;		// index into BoneLookup table or -1.
			EFlags Flags;				//
			int16_t ParentBone;			// index to parent bone or -1.
			uint16_t SubmeshId;			// // Mesh part ID
			uint16_t Unknown[2];			// ?
			M2Track AnimationBlock_Position;	// Float32x3
			M2Track AnimationBlock_Rotation;	// SInt16x4
			M2Track AnimationBlock_Scale;		// Float32x3
			C3Vector Position;
		};

		ASSERT_SIZE(CElement_Bone, 88);

		//
		//
		class CElement_BoneLookup
		{
		public:
			enum EBoneLookup
				: int16_t
			{
				EBoneLookup_ArmL,
				EBoneLookup_ArmR,
				EBoneLookup_ShoulderL,
				EBoneLookup_ShoulderR,
				EBoneLookup_SpineLow,
				EBoneLookup_Waist,
				EBoneLookup_Head,
				EBoneLookup_Jaw,
				EBoneLookup_IndexFingerR,
				EBoneLookup_MiddleFingerR,
				EBoneLookup_PinkyFingerR,
				EBoneLookup_RingFingerR,
				EBoneLookup_ThumbR,
				EBoneLookup_IndexFingerL,
				EBoneLookup_MiddleFingerL,
				EBoneLookup_PinkyFingerL,
				EBoneLookup_RingFingerL,
				EBoneLookup_ThumbL,
				EBoneLookup_Event_BTH,
				EBoneLookup_Event_CSR,
				EBoneLookup_Event_CSL,
				EBoneLookup_Breath,
				EBoneLookup_Name,
				EBoneLookup_NameMount,
				EBoneLookup_Event_CHD,
				EBoneLookup_Event_CCH,
				EBoneLookup_Root,
				EBoneLookup_Wheel1,
				EBoneLookup_Wheel2,
				EBoneLookup_Wheel3,
				EBoneLookup_Wheel4,
				EBoneLookup_Wheel5,
				EBoneLookup_Wheel6,
				EBoneLookup_Wheel7,
				EBoneLookup_Wheel8,
			};

		public:
			EBoneLookup BoneLookup;
		};

		ASSERT_SIZE(CElement_BoneLookup, 2);

		//
		//
		class CElement_Color
		{
		public:
			M2Track AnimationBlock_Color;		// Float32x3
			M2Track AnimationBlock_Opacity;		// uint16_t
		};

		ASSERT_SIZE(CElement_Color, 40);

		//
		//
		class CElement_Texture
		{
		public:
			enum class ETextureType
				: uint32_t
			{
				Final_Hardcoded = 0,
				Skin = 1,
				ObjectSkin = 2,
				WeaponBlade = 3,
				WeaponHandle = 4,
				Environment = 5,
				Hair = 6,
				FacialHair = 7,
				SkinExtra = 8,
				UiSkin = 9,
				TaurenMane = 10,
				Monster1 = 11,
				Monster2 = 12,
				Monster3 = 13,
				ItemIcon = 14,
				GuildBackgroundColor = 15,
				GuildEmblemColor = 16,
				GuildBorderColor = 17,
				GuildEmblem = 18
			};

			static std::string GetTypeString(ETextureType Type);

			enum class ETextureFlags
				: uint32_t
			{
				None = 0,
				WrapX = 1,
				WrapY = 2
			};

			ETextureType Type;
			ETextureFlags Flags;
			M2Array TexturePath;	// texture
		};

		ASSERT_SIZE(CElement_Texture, 16);

		//
		//
		class CElement_Transparency
		{
		public:
			M2Track AnimationBlock_Transparency;	// uint16_t

		};

		ASSERT_SIZE(CElement_Transparency, 20);

		//
		// texture coordinate animation block.
		class CElement_UVAnimation
		{
		public:
			M2Track AnimationBlock_Position;	// Float32x3
			M2Track AnimationBlock_Rotation;	// SInt16x4
			M2Track AnimationBlock_Scale;		// Float32x3
		};

		ASSERT_SIZE(CElement_UVAnimation, 60);

		//
		//
		class CElement_TextureReplace
		{
		public:
			int16_t TextureID;
		};

		ASSERT_SIZE(CElement_TextureReplace, 2);

		//
		//
		class CElement_TextureFlag
		{
		public:
			enum EFlags
				: uint16_t
			{
				EFlags_None = 0x00,
				EFlags_Unlit = 0x01,
				EFlags_Unfogged = 0x02,
				EFlags_TwoSided = 0x04,
				EFlags_Billboard = 0x08,
				EFlags_NoZBuffer = 0x10,
				EFlags_Unk6 = 0x40, // shadow batch related
				EFlags_Unk7 = 0x80, // shadow batch related
				EFlags_Unk8 = 0x400, // wod
				EFlags_Unk9 = 0x800, // prevent alpha for custom elements. if set, use (fully) opaque or transparent. (litSphere, shadowMonk) (MoP+) 
			};

			enum EBlend : uint16_t
			{
				EBlend_Opaque = 0,
				EBlend_Mod,
				EBlend_Decal,
				EBlend_Add,
				EBlend_Mod2x,
				EBlend_Fade,
				EBlend_Unknown6,
				EBlend_Unknown7
			};

		public:
			EFlags Flags;
			EBlend Blend;
		};

		ASSERT_SIZE(CElement_TextureFlag, 4);

		//
		// each skin file has one or more bone partitions. each bone partition references into a subset of this skinned bone lookup array.
		class CElement_PartitionedBoneLookup
		{
		public:
			uint16_t BoneIndex;			// index into the model's bone list.
		};

		ASSERT_SIZE(CElement_PartitionedBoneLookup, 2);

		//
		//
		class CElement_TextureLookup
		{
		public:
			uint16_t TextureIndex;
		};

		ASSERT_SIZE(CElement_TextureLookup, 2);

		//
		//
		class CElement_TextureUnits
		{
		public:
			uint16_t Unit;
		};

		ASSERT_SIZE(CElement_TextureUnits, 2);

		//
		//
		class CElement_TransparencyLookup
		{
		public:
			uint16_t TransparencyLookup;
		};

		ASSERT_SIZE(CElement_TransparencyLookup, 2);

		//
		//
		class CElement_UVAnimationLookup
		{
		public:
			uint16_t TextureIndex;
		};

		ASSERT_SIZE(CElement_UVAnimationLookup, 2);

		//
		//
		class CElement_BoundingTriangle
		{
		public:
			uint16_t Index;
		};

		ASSERT_SIZE(CElement_BoundingTriangle, 2);

		//
		//
		class CElement_BoundingVertices
		{
		public:
			C3Vector Position;
		};

		ASSERT_SIZE(CElement_BoundingVertices, 12);

		//
		//
		class CElement_BoundingNormals
		{
		public:
			C3Vector Normal;
		};

		ASSERT_SIZE(CElement_BoundingNormals, 12);

		//
		// an attachment to a bone defines where items and effects will anchor to the rig.
		class CElement_Attachment
		{
		public:
			enum EID
				: uint32_t
			{
				EID_WristL = 0,		// (shield)
				EID_PalmR = 1,
				EID_PalmL = 2,
				EID_ElbowR = 3,
				EID_ElbowL = 4,
				EID_ShoulderR = 5,
				EID_ShoulderL = 6,
				EID_KneeR = 7,
				EID_KneeL = 8,
				EID_HipR = 9,
				EID_HipL = 10,
				EID_Helmet = 11,
				EID_Back = 12,
				EID_ShoulderFlapR = 13,
				EID_ShoulderFlapL = 14,
				EID_ChestBloodFront = 15,
				EID_ChestBloodBack = 16,
				EID_Breath = 17,
				EID_PlayerName = 18,
				EID_Base = 19,
				EID_Head = 20,
				EID_SpellHandL = 21,
				EID_SpellHandR = 22,
				EID_Special1 = 23,
				EID_Special2 = 24,
				EID_Special3 = 25,
				EID_BackWeaponR = 26,
				EID_BackWeaponL = 27,
				EID_BackSheathShield = 28,
				EID_PlayerNameMounted = 29,
				EID_LargeWeaponL = 30,
				EID_LargeWeaponR = 31,
				EID_HipWeaponL = 32,
				EID_HipWeaponR = 33,
				EID_Chest = 34,
				EID_HandArrow = 35,
				EID_Bullet = 36,
				EID_SpellHandOmni = 37,
				EID_SpellHandDirected = 38,
				EID_VechicleSeat1 = 39,
				EID_VechicleSeat2 = 40,
				EID_VechicleSeat3 = 41,
				EID_VechicleSeat4 = 42,
				EID_VechicleSeat5 = 43,
				EID_VechicleSeat6 = 44,
				EID_VechicleSeat7 = 45,
				EID_VechicleSeat8 = 46,
				EID_FootL = 47,
				EID_FootR = 48,
				EID_ShieldNoGlove = 49,
				EID_SpineLow = 50,
				EID_AlteredShoulderR = 51,
				EID_AlteredSoulderL = 52,
				EID_BeltBuckle = 53,
				EID_SheathCrossbow = 54,
				EID_HeadTop = 55	// horns?
			};

		public:
			uint32_t ID;					//
			uint32_t ParentBone;			// parent bone.
			C3Vector Position;			// position relative to parent bone.
			M2Track AnimationBlock_Visibility;	// uint16_t.
		};

		ASSERT_SIZE(CElement_Attachment, 40);

		//
		//
		class CElement_AttachmentLookup
		{
		public:
			enum EAttachmentLookup : uint16_t
			{
				EAttachmentLookup_Item_PalmR,
				EAttachmentLookup_Item_PalmL,
				EAttachmentLookup_Item_ElbowR,
				EAttachmentLookup_Item_ElbowL,
				EAttachmentLookup_Item_ShoulderR,
				EAttachmentLookup_Item_ShoulderL,
				EAttachmentLookup_Item_KneeR,
				EAttachmentLookup_Item_KneeL,
				EAttachmentLookup_Item_HipR,
				EAttachmentLookup_Item_HipL,
				EAttachmentLookup_Item_Helmet,
				EAttachmentLookup_Item_Back,
				EAttachmentLookup_Item_ShoulderExtR,
				EAttachmentLookup_Item_ShoulderExtL,
				EAttachmentLookup_Hit_Front,
				EAttachmentLookup_Hit_Back,
				EAttachmentLookup_Spell_Mouth,
				EAttachmentLookup_Spell_NearHead,
				EAttachmentLookup_Spell_Base,
				EAttachmentLookup_Spell_Head,
				EAttachmentLookup_Spell_Precast1L,
				EAttachmentLookup_Spell_Precast1R,
				EAttachmentLookup_Spell_Precast2L,
				EAttachmentLookup_Spell_Precast2R,
				EAttachmentLookup_Spell_Precast3,
				EAttachmentLookup_Sheath_UpBackR,
				EAttachmentLookup_Sheath_UpBackL,
				EAttachmentLookup_Sheath_CenterBack,
				EAttachmentLookup_Sheath_HipBelly,
				EAttachmentLookup_Sheath_UpDownBack,
				EAttachmentLookup_Sheath_UnknownBack,
				EAttachmentLookup_Sheath_HipL,
				EAttachmentLookup_Sheath_HipR,
				EAttachmentLookup_Sheath_Chest,
				EAttachmentLookup_Unknown_PalmR1,
				EAttachmentLookup_Unknown_PalmR2,
			};

		public:
			EAttachmentLookup AttachmentLookup;

		};

		ASSERT_SIZE(CElement_AttachmentLookup, 2);


		//
		// events define sounds that are played during animation.
		class CElement_Event
		{
		public:
			class CTimeLine
			{
			public:
				uint32_t nTimeStamps;
				uint32_t oTimeStamps;
			};

		public:
			char ID[4];			// this event's ID.
			uint32_t SoundID;			// database id of sound to play from SoundEntries.dbc.
			uint32_t ParentBone;		// parent bone.
			C3Vector Position;		// position relative to parent bone.
			uint16_t InterpolationType;
			int16_t GlobalSequenceID;
			M2Array TimeLines;
		};

		ASSERT_SIZE(CElement_Event, 36);

		//
		//
		class CElement_Light
		{
		public:
			uint16_t Type;
			uint16_t ParentBone;
			C3Vector Position;
			M2Track AnimationBlock_AmbientColor;		// Float32x3
			M2Track AnimationBlock_AmbientIntensity;	// float
			M2Track AnimationBlock_DiffuseColor;		// Float32x3
			M2Track AnimationBlock_DiffuseIntensity;	// float
			M2Track AnimationBlock_AttenuationStart;	// float
			M2Track AnimationBlock_AttenuationEnd;		// float
			M2Track AnimationBlock_Visibility;			// uint16_t
		};

		ASSERT_SIZE(CElement_Light, 156);

		//
		//
		class CElement_Camera
		{
		public:
			enum ECameraType : int32_t
			{
				FlyBy = -1,
				Portrait = 0,
				PaperDoll = 1
			};

			ECameraType Type;
			float ClipFar;
			float ClipNear;
			M2Track AnimationBlock_Position;		// Float32x3
			C3Vector Position;
			M2Track AnimationBlock_Target;			// Float32x3
			C3Vector Target;
			M2Track AnimationBlock_Roll;			// float
			M2Track AnimationBlock_FieldOfView;		// float	// v4
		};

		ASSERT_SIZE(CElement_Camera, 116);

		class CElement_Camera_PreCata
		{
		public:
			enum ECameraType : int32_t
			{
				FlyBy = -1,
				Portrait = 0,
				PaperDoll = 1
			};

			ECameraType Type;
			float FoV;
			float ClipFar;
			float ClipNear;
			M2Track AnimationBlock_Position;		// Float32x3
			C3Vector Position;
			M2Track AnimationBlock_Target;			// Float32x3
			C3Vector Target;
			M2Track AnimationBlock_Roll;			// float
		};

		ASSERT_SIZE(CElement_Camera_PreCata, 100);

		//
		//
		class CElement_CameraLookup
		{
		public:
			uint16_t Index;
		};

		ASSERT_SIZE(CElement_CameraLookup, 2);

		//
		//
		class CElement_RibbonEmitter
		{
		public:
			int32_t ID;
			int32_t ParentBone;
			C3Vector Position;
			M2Array Texture;
			M2Array RenderFlag;
			M2Track AnimationBlock_Color;		// Float32x3
			M2Track AnimationBlock_Opacity;		// uint16_t
			M2Track AnimationBlock_HeightAbove;	// float, position of point A of ribbon leading edge.
			M2Track AnimationBlock_HeightBelow;	// float, position of point B of ribbon leading edge.
			float EdgesPerSecond;
			float EdgeLifetime;
			float EmissionAngle;		// use arcsin(val) to get the angle in degree.
			uint16_t TextureRows;
			uint16_t TextureCols;
			M2Track AnimationBlock_TexSlotTrack;	// int32_t
			M2Track AnimationBlock_Visibility;	// int16_t
			int16_t PriorityPlane;
			uint16_t Padding;
		};

		ASSERT_SIZE(CElement_RibbonEmitter, 176);

		//
		// 492 0x1EC
		class CElement_ParticleEmitter
		{
		public:
			uint32_t ID;				// always -1?
			uint32_t Flags;
			C3Vector Position;		// position relative to parent bone.
			uint16_t ParentBone;
			uint16_t Texture;
			M2Array FileNameModel;	// name of model to spawn *.mdx.
			M2Array ChildEmitter;

			uint8_t BlendingType;
			uint8_t EmitterType;
			uint16_t ParticleColorIndex;

			fixed_point<uint8_t, 2, 5> multiTextureParamX[2];

			uint16_t TextureTileRotation;
			uint16_t TextureDimensionsRows;
			uint16_t TextureDimensionsColumns;

			M2Track AnimationBlock_EmitSpeed;		// float
			M2Track AnimationBlock_SpeedVariance;	// float
			M2Track AnimationBlock_VerticalRange;	// float
			M2Track AnimationBlock_HorizontalRange;	// float
			M2Track AnimationBlock_Gravity;			// float
			M2Track AnimationBlock_Lifespan;		// float
			float LifespanVary;
			M2Track AnimationBlock_EmissionRate;	// float
			float EmissionRateVary;
			M2Track AnimationBlock_EmissionAreaLength;	// float
			M2Track AnimationBlock_EmissionAreaWidth;	// float
			M2Track AnimationBlock_zSource;			// float

			CElement_FakeAnimationBlock ColorTrack;
			CElement_FakeAnimationBlock AlphaTrack;
			CElement_FakeAnimationBlock ScaleTrack;
			C2Vector ScaleVary;
			CElement_FakeAnimationBlock HeadCellTrack;
			CElement_FakeAnimationBlock TailCellTrack;
			float TailLength;
			float TwinkleSpeed;
			float TwinklePercent;
			CRange TwinkleScale;
			float BurstMultiplier;
			float Drag;
			float BaseSpin;
			float BaseSpinVary;
			float Spin;
			float SpinVary;

			M2Box Tumble;

			C3Vector WindVector;
			float WindTime;
			
			float FollowSpeed1;
			float FollowScale1;
			float FollowSpeed2;
			float FollowScale2;

			M2Array SplinePoints;	// C3Vector
			M2Track AnimationBlock_EnabledIn;		// uint16_t

			vector_2fp_6_9 multiTextureParam0[2];
			vector_2fp_6_9 multiTextureParam1[2];
		};

		ASSERT_SIZE(CElement_ParticleEmitter, 492);

#pragma pack(pop)
	}
}

