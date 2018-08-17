#pragma once

// a lot of helpful information:
// http://madx.dk/wowdev/wiki/index.php?title=M2/WotLK
// http://wowdev.org/wiki/index.php/M2

#include "BaseTypes.h"
#include "M2Types.h"
#include "M2I.h"
#include "DataElement.h"
#include "M2Element.h"
#include "M2Skin.h"
#include "M2Chunk.h"

#define DegreesToRadians 0.0174532925f

namespace M2Lib
{
	class Casc;
	class GlobalSettings;
	class Skeleton;

	// load, export, import merge, save: M2 file.
	class M2
	{
	public:

#pragma pack(push,1)
		// memory mapped header.
		class CM2Header
		{
		public:
			struct CM2Description
			{
				Char8 ID[4];	// 'MD20'
				UInt32 Version;

				UInt32 nName;
				UInt32 oName;

				UInt32 Flags;
			} Description;

			ASSERT_SIZE(CM2Description, 20);

			struct CM2Elements
			{
				UInt32 nGlobalSequence;		// 4
				UInt32 oGlobalSequence;

				UInt32 nAnimation;			// 64
				UInt32 oAnimation;

				UInt32 nAnimationLookup;	// 2
				UInt32 oAnimationLookup;

				UInt32 nBone;				// 88
				UInt32 oBone;

				UInt32 nKeyBoneLookup;		// 2
				UInt32 oKeyBoneLookup;

				UInt32 nVertex;				// 48
				UInt32 oVertex;

				UInt32 nSkin;				// 

				UInt32 nColor;				// 40
				UInt32 oColor;

				UInt32 nTexture;			// 16
				UInt32 oTexture;

				UInt32 nTransparency;		// 20
				UInt32 oTransparency;

				UInt32 nTextureAnimation;	// 60
				UInt32 oTextureAnimation;

				UInt32 nTextureReplace;		// 2
				UInt32 oTextureReplace;

				UInt32 nTextureFlags;		// 4
				UInt32 oTextureFlags;

				UInt32 nSkinnedBoneLookup;	// 2
				UInt32 oSkinnedBoneLookup;

				UInt32 nTextureLookup;		// 2
				UInt32 oTextureLookup;

				UInt32 nTextureUnitLookup;	// 2
				UInt32 oTextureUnitLookup;

				UInt32 nTransparencyLookup;	// 2
				UInt32 oTransparencyLookup;

				UInt32 nTextureAnimationLookup;	// 2
				UInt32 oTextureAnimationLookup;

				SVolume CollisionVolume;
				SVolume BoundingVolume;

				UInt32 nBoundingTriangle;		// 2
				UInt32 oBoundingTriangle;

				UInt32 nBoundingVertex;			// 12
				UInt32 oBoundingVertex;

				UInt32 nBoundingNormal;			// 12
				UInt32 oBoundingNormal;

				UInt32 nAttachment;				// 40
				UInt32 oAttachment;

				UInt32 nAttachmentLookup;		// 2
				UInt32 oAttachmentLookup;

				UInt32 nEvent;					// 36
				UInt32 oEvent;

				UInt32 nLight;					// 156
				UInt32 oLight;

				UInt32 nCamera;					// 116
				UInt32 oCamera;

				UInt32 nCameraLookup;			// 2
				UInt32 oCameraLookup;

				UInt32 nRibbonEmitter;			// 176
				UInt32 oRibbonEmitter;

				UInt32 nParticleEmitter;		// 492
				UInt32 oParticleEmitter;

				UInt32 nUnknown1;
				UInt32 oUnknown1;

			} Elements;

			ASSERT_SIZE(CM2Elements, 292);

			bool IsLongHeader() const;
		};

#pragma pack(pop)

	private:
		std::wstring _FileName;	// needed to create skin file names so we can load/save skins.

		std::map<M2Chunk::EM2Chunk, ChunkBase*> Chunks;

		Skeleton* Skeleton;
		bool hasLodSkins;

		UInt32 m_OriginalModelChunkSize;
		GlobalSettings* Settings;

		M2I* pInM2I;
		M2* replaceM2;
		Casc* casc;

	public:
		CM2Header Header;		// used for loading and saving. not used when editing.
		DataElement Elements[M2Element::EElement__Count__];
		DataElement* GetLastElement();

		#define SKIN_COUNT 7
		#define LOD_SKIN_MAX_COUNT 3
		M2Skin* Skins[SKIN_COUNT];
		UInt32 OriginalSkinCount;	// skin count before importing m2i

		M2(GlobalSettings* Settings = NULL)
		{
			memset(Skins, 0, sizeof(Skins));
			Skeleton = NULL;
			m_OriginalModelChunkSize = 0;
			pInM2I = NULL;
			replaceM2 = NULL;
			hasLodSkins = false;
			this->Settings = Settings;
			casc = NULL;
			OriginalSkinCount = 0;
		}

		~M2()
		{
			if (pInM2I)
				delete pInM2I;
			for (auto& Chunk : Chunks)
				delete Chunk.second;
		}

		UInt32 GetHeaderSize() const;
		Expansion GetExpansion() const;

		std::wstring GetModelDirectory() const;

		// loads an M2 from a file.
		EError Load(const Char16* FileName);

		// saves this M2 to a file.
		EError Save(const Char16* FileName, M2* replaceM2 = NULL);

		// exports the loaded M2 as an M2I file.
		EError ExportM2Intermediate(Char16 const* FileName);
		// imports an M2I file and merges it with already loaded M2.
		EError ImportM2Intermediate(Char16 const* FileName);
		
		// prints diagnostic information.
		void PrintInfo();
		void PrintReferencedFileInfo();

		void CopySFIDChunk(M2* Other);
		UInt32 AddBone(M2Element::CElement_Bone const& Bone);
		UInt32 AddTexture(const Char8* szTextureSource, M2Element::CElement_Texture::ETextureType Type, M2Element::CElement_Texture::ETextureFlags Flags);
		UInt32 AddTextureLookup(UInt16 TextureId, bool ForceNewIndex = false);
		UInt32 GetTextureIndex(const Char8* szTextureSource);
		std::string GetTexturePath(UInt32 Index);

		Casc* GetCasc() const { return casc; }
		void SetCasc(Casc* casc) { this->casc = casc; }

		DataElement* GetAnimations();
		DataElement* GetBones();
		DataElement* GetBoneLookups();
		DataElement* GetAttachments();

	private:
		// utilities and tests

		// averages normals of duplicate vertices within submeshes.
		void FixSeamsSubMesh(Float32 PositionalTolerance, Float32 AngularTolerance);
		// averages normals of duplicate vertices between body submeshes.
		void FixSeamsBody(Float32 PositionalTolerance, Float32 AngularTolerance);
		// closes small gaps between clothing and body and copies normals from body to clothing.
		void FixSeamsClothing(Float32 PositionalTolerance, Float32 AngularTolerance);

		void DoExtraWork();

		// scales all positional coordinates of vertices, bones, attachments, events, lights, cameras, ribbon emitters, particle emitters.
		void Scale(Float32 Scale);
		// mirrors the portrait camera accross the Y axis
		void MirrorCamera();

		void SetGlobalBoundingData(BoundaryData& Data);

		UInt32 CloneTexture(UInt16 TextureId);
		UInt32 AddTextureFlags(M2Element::CElement_TextureFlag::EFlags Flags, M2Element::CElement_TextureFlag::EBlend Blend);

		EError LoadSkins();
		EError SaveSkins(wchar_t const* M2FileName);

		bool GetFileSkin(std::wstring& SkinFileNameResultBuffer, std::wstring const& M2FileName, UInt32 SkinIndex);
		bool GetFileSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName);

		ChunkBase* GetChunk(M2Chunk::EM2Chunk ChunkId);
		void RemoveChunk(M2Chunk::EM2Chunk ChunkId);
		void PrepareChunks();

		// post load header
		void m_LoadElements_CopyHeaderToElements();
		void m_LoadElements_FindSizes(UInt32 ChunkSize);

		// pre save header
		void m_SaveElements_FindOffsets();
		void m_FixAnimationOffsets(SInt32 OffsetDelta, SInt32 TotalDelta, M2Element::CElement_AnimationBlock& AnimationBlock, SInt32 iElement);
		void m_FixAnimationM2Array(SInt32 OffsetDelta, SInt32 TotalDelta, SInt16 GlobalSequenceID, M2Array& Array, SInt32 iElement);
		void m_FixFakeAnimationBlockOffsets(SInt32 OffsetDelta, M2Element::CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement);

		void m_SaveElements_CopyElementsToHeader();
	};
}
