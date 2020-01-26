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
#include "Settings.h"
#include <unordered_map>

#define DegreesToRadians 0.0174532925f

namespace M2Lib
{
	class FileStorage;
	struct FileInfo;
	struct Settings;
	class Skeleton;

	namespace SkeletonChunk
	{
		class AFIDChunk;
	}

	enum SaveFlags
	{
		SAVE_NONE = 0,
		SAVE_M2 = 1,	// always on
		SAVE_SKIN = 2,
		SAVE_SKELETON = 4,
		SAVE_OTHER = 8,
		SAVE_ALL = SAVE_M2 | SAVE_SKIN | SAVE_SKELETON | SAVE_OTHER,
	};

	M2LIB_API M2LIB_HANDLE __cdecl M2_Create(Settings* settings = nullptr);
	M2LIB_API EError __cdecl M2_Load(M2LIB_HANDLE handle, const wchar_t* FileName);
	M2LIB_API EError __cdecl M2_Save(M2LIB_HANDLE handle, const wchar_t* FileName, uint8_t saveMask);
	M2LIB_API EError __cdecl M2_SetReplaceM2(M2LIB_HANDLE handle, const wchar_t* FileName);
	M2LIB_API EError __cdecl M2_ExportM2Intermediate(M2LIB_HANDLE handle, const wchar_t* FileName);
	M2LIB_API EError __cdecl M2_ImportM2Intermediate(M2LIB_HANDLE handle, const wchar_t* FileName);
	M2LIB_API EError __cdecl M2_SetNeedRemapReferences(M2LIB_HANDLE handle, const wchar_t* remapPath);
	M2LIB_API EError __cdecl M2_SetNeedRemoveTXIDChunk(M2LIB_HANDLE handle);
	M2LIB_API void __cdecl M2_Free(M2LIB_HANDLE handle);

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
				char ID[4];	// 'MD20'
				uint32_t Version;

				uint32_t nName;
				uint32_t oName;

				union
				{
					uint32_t Raw;
					struct {
						uint32_t flag_tilt_x : 1;
						uint32_t flag_tilt_y : 1;
						uint32_t flag_unk_0x4 : 1;
						uint32_t flag_use_texture_combiner_combos : 1; // add textureCombinerCombos array to end of data
						uint32_t flag_unk_0x10 : 1;
						uint32_t flag_load_phys_data : 1;
						uint32_t flag_unk_0x40 : 1;
						uint32_t flag_unk_0x80 : 1; // with this flag unset, demon hunter tattoos stop glowing
						uint32_t flag_camera_related : 1;
						uint32_t flag_new_particle_record : 1; // In CATA: new version of ParticleEmitters. By default, length of M2ParticleOld is 476. But if 0x200 is set or if version is bigger than 271, length of M2ParticleOld is 492.
						uint32_t flag_unk_0x400 : 1;
						uint32_t flag_texture_transforms_use_bone_sequences : 1; // >= WoD 0x800 -- When set, texture transforms are animated using the sequence being played on the bone found by index in tex_unit_lookup_table[textureTransformIndex], instead of using the sequence being played on the model's first bone. Example model: 6DU_HellfireRaid_FelSiege03_Creature
						uint32_t flag_unk_0x1000 : 1;
						uint32_t flag_unk_0x2000 : 1; // seen in various legion models
						uint32_t flag_unk_0x4000 : 1;
						uint32_t flag_unk_0x8000 : 1; // seen in UI_MainMenu_Legion
						uint32_t flag_unk_0x10000 : 1;
						uint32_t flag_unk_0x20000 : 1;
						uint32_t flag_unk_0x40000 : 1;
						uint32_t flag_unk_0x80000 : 1;
						uint32_t flag_unk_0x100000 : 1;
						uint32_t flag_unk_0x200000 : 1; // apparently: use 24500 upgraded model format: chunked .anim files, change in the exporter reordering sequence+bone blocks before name
					};
				} Flags;
			} Description;

			ASSERT_SIZE(CM2Description, 20);

			struct CM2Elements
			{
				uint32_t nGlobalSequence;		// 4
				uint32_t oGlobalSequence;

				uint32_t nAnimation;			// 64
				uint32_t oAnimation;

				uint32_t nAnimationLookup;	// 2
				uint32_t oAnimationLookup;

				uint32_t nBone;				// 88
				uint32_t oBone;

				uint32_t nKeyBoneLookup;		// 2
				uint32_t oKeyBoneLookup;

				uint32_t nVertex;				// 48
				uint32_t oVertex;

				uint32_t nSkin;				// 

				uint32_t nColor;				// 40
				uint32_t oColor;

				uint32_t nTexture;			// 16
				uint32_t oTexture;

				uint32_t nTransparency;		// 20
				uint32_t oTransparency;

				uint32_t nTextureAnimation;	// 60
				uint32_t oTextureAnimation;

				uint32_t nTextureReplace;		// 2
				uint32_t oTextureReplace;

				uint32_t nTextureFlags;		// 4
				uint32_t oTextureFlags;

				uint32_t nSkinnedBoneLookup;	// 2
				uint32_t oSkinnedBoneLookup;

				uint32_t nTextureLookup;		// 2
				uint32_t oTextureLookup;

				uint32_t nTextureUnitLookup;	// 2
				uint32_t oTextureUnitLookup;

				uint32_t nTransparencyLookup;	// 2
				uint32_t oTransparencyLookup;

				uint32_t nTextureAnimationLookup;	// 2
				uint32_t oTextureAnimationLookup;

				SVolume CollisionVolume;
				SVolume BoundingVolume;

				uint32_t nBoundingTriangle;		// 2
				uint32_t oBoundingTriangle;

				uint32_t nBoundingVertex;			// 12
				uint32_t oBoundingVertex;

				uint32_t nBoundingNormal;			// 12
				uint32_t oBoundingNormal;

				uint32_t nAttachment;				// 40
				uint32_t oAttachment;

				uint32_t nAttachmentLookup;		// 2
				uint32_t oAttachmentLookup;

				uint32_t nEvent;					// 36
				uint32_t oEvent;

				uint32_t nLight;					// 156
				uint32_t oLight;

				uint32_t nCamera;					// 116
				uint32_t oCamera;

				uint32_t nCameraLookup;			// 2
				uint32_t oCameraLookup;

				uint32_t nRibbonEmitter;			// 176
				uint32_t oRibbonEmitter;

				uint32_t nParticleEmitter;		// 492
				uint32_t oParticleEmitter;

				uint32_t nTextureCombinerCombo;
				uint32_t oTextureCombinerCombo;

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

		bool needRemapReferences;
		std::wstring remapPath;
		std::map<uint32_t, uint32_t> remapCopyFiles;

		bool needRemoveTXIDChunk; // TXID chunk will be removed when model has textures that are not indexed in CASC storage

		uint32_t m_OriginalModelChunkSize;
		Settings Settings;
		FileStorage* storageRef;

		uint32_t GetCustomMappingCounter();
		FileInfo const* AddCustomMapping(wchar_t const* path);
		uint32_t currentCustomFileDataId = 0;
		std::map<uint32_t, FileInfo const*> customFileInfosByFileDataId;
		std::map<uint64_t, FileInfo const*> customFileInfosByNameHash;

		M2I* pInM2I;
		M2* replaceM2;

	public:
		CM2Header Header;		// used for loading and saving. not used when editing.
		DataElement Elements[M2Element::EElement__CountM2__];
		uint32_t GetLastElementIndex() const;
		M2Lib::Settings const* GetSettings() const;
		wchar_t const* GetFileName() const { return _FileName.c_str(); }

#define SKIN_COUNT 7
		#define LOD_SKIN_MAX_COUNT 3
		M2Skin* Skins[SKIN_COUNT];
		uint32_t OriginalSkinCount;	// skin count before importing m2i

		M2(M2Lib::Settings* Settings = NULL);

		~M2();

		uint32_t GetHeaderSize() const;
		Expansion GetExpansion() const;

		// loads an M2 from a file.
		EError Load(const wchar_t* FileName);

		EError SetReplaceM2(const wchar_t* FileName);

		// saves this M2 to a file.
		EError Save(const wchar_t* FileName, uint8_t saveMask);

		// exports the loaded M2 as an M2I file.
		EError ExportM2Intermediate(wchar_t const* FileName);
		// imports an M2I file and merges it with already loaded M2.
		EError ImportM2Intermediate(wchar_t const* FileName);
		
		// prints diagnostic information.
		void PrintInfo();
		void PrintReferencedFileInfo();

		bool RemapReferences(const wchar_t* m2FileName);
		void CopyRemappedFiles(const wchar_t* m2FileName);

		uint32_t AddBone(M2Element::CElement_Bone const& Bone);
		uint32_t AddTexture(M2Element::CElement_Texture::ETextureType Type, M2Element::CElement_Texture::ETextureFlags Flags, char const* szTextureSource);
		uint32_t AddTextureLookup(uint16_t TextureId, bool ForceNewIndex = false);
		uint32_t GetTextureIndex(M2Element::CElement_Texture::ETextureType Type, const char* szTextureSource);
		std::wstring GetTexturePath(uint32_t Index);
		void RemoveTXIDChunk();
		EError SetNeedRemoveTXIDChunk();
		EError SetNeedRemapReferences(const wchar_t* remapPath);

		DataElement* GetAnimations();
		DataElement* GetAnimationsLookup();
		DataElement* GetBones();
		DataElement* GetBoneLookups();
		DataElement* GetAttachments();

		SkeletonChunk::AFIDChunk* GetSkeletonAFIDChunk();

	private:
		// utilities and tests

		// averages normals of duplicate vertices within submeshes.
		void FixSeamsSubMesh(float PositionalTolerance, float AngularTolerance);
		// averages normals of duplicate vertices between body submeshes.
		void FixSeamsBody(float PositionalTolerance, float AngularTolerance);
		// closes small gaps between clothing and body and copies normals from body to clothing.
		void FixSeamsClothing(float PositionalTolerance, float AngularTolerance);
		// averages normals on mesh edges
		void FixNormals(float AngularTolerance);
		void FixNormals(std::list<M2SkinElement::CElement_SubMesh const*> const& source, std::list<M2SkinElement::CElement_SubMesh const*> const& target, float AngularTolerance, bool preferSource);

		void DoExtraWork();

		// scales all positional coordinates of vertices, bones, attachments, events, lights, cameras, ribbon emitters, particle emitters.
		void Scale(float Scale);
		// mirrors the portrait camera accross the Y axis
		void MirrorCamera();

		void SetGlobalBoundingData(BoundaryData& Data);

		uint32_t CloneTexture(uint16_t TextureId);
		uint32_t AddTextureFlags(M2Element::CElement_TextureFlag::EFlags Flags, M2Element::CElement_TextureFlag::EBlend Blend);

		EError LoadSkins();
		EError SaveSkins(wchar_t const* M2FileName);

		bool GetFileSkin(std::wstring& SkinFileNameResultBuffer, std::wstring const& M2FileName, uint32_t SkinIndex, bool Save);
		bool GetFileSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName, bool Save);
		bool GetFileParentSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName, bool Save) const;
		std::wstring BuildFsPath(std::wstring const& path);

		EError LoadSkeleton();
		EError SaveSkeleton(std::wstring const& M2FileName);

		EError SaveCustomMappings(wchar_t const* fileName);

		ChunkBase* GetChunk(M2Chunk::EM2Chunk ChunkId);
		void RemoveChunk(M2Chunk::EM2Chunk ChunkId);

		void CopyReplaceChunks();
		void FixSkinChunk();

		// post load header
		void m_LoadElements_CopyHeaderToElements();
		void m_LoadElements_FindSizes(uint32_t ChunkSize);

		// pre save header
		void m_SaveElements_FindOffsets();

		void m_FixAnimationOffsets(int32_t OffsetDelta, int32_t TotalDelta, M2Element::M2Track& AnimationBlock, int32_t iElement);
		void m_FixAnimationM2Array(int32_t OffsetDelta, int32_t TotalDelta, int16_t GlobalSequenceID, M2Array& Array, int32_t iElement);
		void m_FixFakeAnimationBlockOffsets(int32_t OffsetDelta, int32_t TotalDelta, M2Element::CElement_FakeAnimationBlock& AnimationBlock, int32_t iElement);

		void m_FixAnimationOffsets_Old(int32_t OffsetDelta, int32_t TotalDelta, M2Element::M2Track& AnimationBlock, int32_t iElement);
		void m_FixAnimationM2Array_Old(int32_t OffsetDelta, int32_t TotalDelta, int16_t GlobalSequenceID, M2Array& Array, int32_t iElement);
		void m_FixFakeAnimationBlockOffsets_Old(int32_t OffsetDelta, int32_t TotalDelta, M2Element::CElement_FakeAnimationBlock& AnimationBlock, int32_t iElement);

		void m_SaveElements_CopyElementsToHeader();

		FileInfo const* GetFileInfoByFileDataId(uint32_t FileDataId) const;
		FileInfo const* GetFileInfoByPath(std::wstring const& Path) const;
		FileInfo const* GetFileInfoByPartialPath(std::wstring const& Path) const;
		wchar_t const* PathInfo(uint32_t FileDataId) const;
	};
}

