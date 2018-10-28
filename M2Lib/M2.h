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

	namespace SkeletonChunk
	{
		class AFIDChunk;
	}

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

				union
				{
					UInt32 Raw;
					struct {
						UInt32 flag_tilt_x : 1;
						UInt32 flag_tilt_y : 1;
						UInt32 flag_unk_0x4 : 1;
						UInt32 flag_use_texture_combiner_combos : 1; // add textureCombinerCombos array to end of data
						UInt32 flag_unk_0x10 : 1;
						UInt32 flag_load_phys_data : 1;
						UInt32 flag_unk_0x40 : 1;
						UInt32 flag_unk_0x80 : 1; // with this flag unset, demon hunter tattoos stop glowing
						UInt32 flag_camera_related : 1;
						UInt32 flag_new_particle_record : 1; // In CATA: new version of ParticleEmitters. By default, length of M2ParticleOld is 476. But if 0x200 is set or if version is bigger than 271, length of M2ParticleOld is 492.
						UInt32 flag_unk_0x400 : 1;
						UInt32 flag_texture_transforms_use_bone_sequences : 1; // >= WoD 0x800 -- When set, texture transforms are animated using the sequence being played on the bone found by index in tex_unit_lookup_table[textureTransformIndex], instead of using the sequence being played on the model's first bone. Example model: 6DU_HellfireRaid_FelSiege03_Creature
						UInt32 flag_unk_0x1000 : 1;
						UInt32 flag_unk_0x2000 : 1; // seen in various legion models
						UInt32 flag_unk_0x4000 : 1;
						UInt32 flag_unk_0x8000 : 1; // seen in UI_MainMenu_Legion
						UInt32 flag_unk_0x10000 : 1;
						UInt32 flag_unk_0x20000 : 1;
						UInt32 flag_unk_0x40000 : 1;
						UInt32 flag_unk_0x80000 : 1;
						UInt32 flag_unk_0x100000 : 1;
						UInt32 flag_unk_0x200000 : 1; // apparently: use 24500 upgraded model format: chunked .anim files, change in the exporter reordering sequence+bone blocks before name
					};
				} Flags;
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

				UInt32 nTextureCombinerCombo;
				UInt32 oTextureCombinerCombo;

			} Elements;

			ASSERT_SIZE(CM2Elements, 292);

			bool IsLongHeader() const;
		};

#pragma pack(pop)

	private:
		std::wstring _FileName;	// needed to create skin file names so we can load/save skins.

		std::map<M2Chunk::EM2Chunk, ChunkBase*> Chunks;

		M2Lib::Skeleton* Skeleton;
		M2Lib::Skeleton* ParentSkeleton;
		bool hasLodSkins;
		bool needRemoveTXIDChunk; // TXID chunk will be removed when model has textures that are not indexed in CASC storage

		UInt32 m_OriginalModelChunkSize;
		GlobalSettings* Settings;

		M2I* pInM2I;
		M2* replaceM2;
		Casc* casc;

	public:
		CM2Header Header;		// used for loading and saving. not used when editing.
		DataElement Elements[M2Element::EElement__Count__];
		UInt32 GetLastElementIndex();

		#define SKIN_COUNT 7
		#define LOD_SKIN_MAX_COUNT 3
		M2Skin* Skins[SKIN_COUNT];
		UInt32 OriginalSkinCount;	// skin count before importing m2i

		M2(GlobalSettings* Settings = NULL)
		{
			memset(Skins, 0, sizeof(Skins));
			Skeleton = NULL;
			ParentSkeleton = NULL;
			m_OriginalModelChunkSize = 0;
			pInM2I = NULL;
			replaceM2 = NULL;
			hasLodSkins = false;
			needRemoveTXIDChunk = false;
			this->Settings = Settings;
			casc = NULL;
			OriginalSkinCount = 0;
		}

		~M2();

		UInt32 GetHeaderSize() const;
		Expansion GetExpansion() const;

		// loads an M2 from a file.
		EError Load(const Char16* FileName);

		void SetReplaceM2(M2* replaceM2 = NULL) { this->replaceM2 = replaceM2; }

		// saves this M2 to a file.
		EError Save(const Char16* FileName);

		// exports the loaded M2 as an M2I file.
		EError ExportM2Intermediate(Char16 const* FileName);
		// imports an M2I file and merges it with already loaded M2.
		EError ImportM2Intermediate(Char16 const* FileName);
		
		// prints diagnostic information.
		void PrintInfo();
		void PrintReferencedFileInfo();

		UInt32 AddBone(M2Element::CElement_Bone const& Bone);
		UInt32 AddTexture(M2Element::CElement_Texture::ETextureType Type, M2Element::CElement_Texture::ETextureFlags Flags, char const* szTextureSource);
		UInt32 AddTextureLookup(UInt16 TextureId, bool ForceNewIndex = false);
		UInt32 GetTextureIndex(M2Element::CElement_Texture::ETextureType Type, const Char8* szTextureSource);
		std::string GetTexturePath(UInt32 Index);
		void RemoveTXIDChunk();

		Casc* GetCasc() const { return casc; }
		void SetCasc(Casc* casc);

		DataElement* GetAnimations();
		DataElement* GetAnimationsLookup();
		DataElement* GetBones();
		DataElement* GetBoneLookups();
		DataElement* GetAttachments();

		SkeletonChunk::AFIDChunk* GetSkeletonAFIDChunk();

	private:
		// utilities and tests

		// averages normals of duplicate vertices within submeshes.
		void FixSeamsSubMesh(Float32 PositionalTolerance, Float32 AngularTolerance);
		// averages normals of duplicate vertices between body submeshes.
		void FixSeamsBody(Float32 PositionalTolerance, Float32 AngularTolerance);
		// closes small gaps between clothing and body and copies normals from body to clothing.
		void FixSeamsClothing(Float32 PositionalTolerance, Float32 AngularTolerance);
		// averages normals on mesh edges
		void FixNormals(Float32 AngularTolerance);

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

		bool GetFileSkin(std::wstring& SkinFileNameResultBuffer, std::wstring const& M2FileName, UInt32 SkinIndex, bool Save);
		bool GetFileSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName, bool Save);
		bool GetFileParentSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName, bool Save);

		EError LoadSkeleton();
		EError SaveSkeleton(std::wstring const& M2FileName);

		ChunkBase* GetChunk(M2Chunk::EM2Chunk ChunkId);
		void RemoveChunk(M2Chunk::EM2Chunk ChunkId);

		void CopyReplaceChunks();
		void FixSkinChunk();

		// post load header
		void m_LoadElements_CopyHeaderToElements();
		void m_LoadElements_FindSizes(UInt32 ChunkSize);

		// pre save header
		void m_SaveElements_FindOffsets();

		void m_FixAnimationOffsets(SInt32 OffsetDelta, SInt32 TotalDelta, M2Element::CElement_AnimationBlock& AnimationBlock, SInt32 iElement);
		void m_FixAnimationM2Array(SInt32 OffsetDelta, SInt32 TotalDelta, SInt16 GlobalSequenceID, M2Array& Array, SInt32 iElement);
		void m_FixFakeAnimationBlockOffsets(SInt32 OffsetDelta, SInt32 TotalDelta, M2Element::CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement);

		void m_FixAnimationOffsets_Old(SInt32 OffsetDelta, SInt32 TotalDelta, M2Element::CElement_AnimationBlock& AnimationBlock, SInt32 iElement);
		void m_FixAnimationM2Array_Old(SInt32 OffsetDelta, SInt32 TotalDelta, SInt16 GlobalSequenceID, M2Array& Array, SInt32 iElement);
		void m_FixFakeAnimationBlockOffsets_Old(SInt32 OffsetDelta, SInt32 TotalDelta, M2Element::CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement);

		void m_SaveElements_CopyElementsToHeader();
	};
}

