#include "M2.h"
#include "DataBinary.h"
#include "M2SkinBuilder.h"
#include "Settings.h"
#include "Skeleton.h"
#include "FileStorage.h"
#include "Logger.h"
#include <sstream>
#include <set>
#include "StringHelpers.h"
#include "StringHash.h"
#include <filesystem>

using namespace M2Lib::M2Element;
using namespace M2Lib::M2Chunk;

uint32_t M2Lib::M2::GetLastElementIndex()
{
	for (int32_t i = M2Element::EElement__Count__ - 1; i >= 0; --i)
	{
		if (!Elements[i].Data.empty())
			return i;
	}

	return M2Element::EElement__Count__;
}

M2Lib::Settings const* M2Lib::M2::GetSettings()
{
	return &Settings;
}

M2Lib::Expansion M2Lib::M2::GetExpansion() const
{
	if (Settings.ForceLoadExpansion  != Expansion::None)
		return Settings.ForceLoadExpansion;

	if (Header.Description.Version < 264)
		return Expansion::BurningCrusade;
	if (Header.Description.Version == 264)
		return Expansion::WrathOfTheLichKing;
	if (Header.Description.Version < 272)
		return Expansion::Cataclysm;
	if (Header.Description.Version < 274)
		return Expansion::WarlordsOfDraenor;

	return Expansion::Legion;
}

bool M2Lib::M2::CM2Header::IsLongHeader() const
{
	return Description.Flags.flag_use_texture_combiner_combos;
}

M2Lib::M2::M2(M2Lib::Settings* Settings)
{
	memset(Skins, 0, sizeof(Skins));
	Skeleton = NULL;
	ParentSkeleton = NULL;
	m_OriginalModelChunkSize = 0;
	pInM2I = NULL;
	replaceM2 = NULL;
	hasLodSkins = false;
	needRemoveTXIDChunk = false;
	if (Settings)
		this->Settings = *Settings;

	storageRef = StorageManager::GetInstance()->GetStorage(this->Settings.MappingsDirectory);

	OriginalSkinCount = 0;
}

M2Lib::M2::~M2()
{
	delete pInM2I;
	for (auto& Chunk : Chunks)
		delete Chunk.second;
	Chunks.clear();
	delete Skeleton;
	delete ParentSkeleton;

	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
		delete Skins[i];

	delete replaceM2;
}

uint32_t M2Lib::M2::GetHeaderSize() const
{
	return Header.IsLongHeader() && GetExpansion() >= Expansion::Cataclysm ? sizeof(Header) : sizeof(Header) - 8;
}

M2Lib::EError M2Lib::M2::Load(const wchar_t* FileName)
{
	// check path
	if (!FileName)
	{
		sLogger.LogError(L"Error: No file specified");
		return EError_FailedToLoadM2_NoFileSpecified;
	}

	_FileName = FileName;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
	{
		sLogger.LogError(L"Error: Failed to open file %s", FileName);
		return EError_FailedToLoadM2_CouldNotOpenFile;
	}

	sLogger.LogInfo(L"Loading model at %s", FileName);

	// find file size
	FileStream.seekg(0, std::ios::end);
	uint32_t FileSize = (uint32_t)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	sLogger.LogInfo(L"File size: %u", FileSize);

	struct PostChunkInfo
	{
		PostChunkInfo() { }
		PostChunkInfo(uint32_t Offs, uint32_t Size)
		{
			this->Offs = Offs;
			this->Size = Size;
		}

		uint32_t Offs;
		uint32_t Size;
	};

	std::map<EM2Chunk, PostChunkInfo> PostProcessChunks;

	while (FileStream.tellg() < FileSize)
	{
		uint32_t ChunkId;
		uint32_t ChunkSize;

		FileStream.read((char*)&ChunkId, sizeof(ChunkId));
		FileStream.read((char*)&ChunkSize, sizeof(ChunkSize));

		// support pre-legion M2
		if (REVERSE_CC(ChunkId) == 'MD20')
		{
			sLogger.LogInfo(L"Detected pre-Legion mode (unchunked)");
			auto Chunk = new MD21Chunk();
			FileStream.seekg(0, std::ios::beg);
			Chunk->Load(FileStream, FileSize);
			Chunks[EM2Chunk::Model] = Chunk;
			break;
		}
		else
		{
			ChunkBase* Chunk = NULL;
			auto eChunk = (EM2Chunk)REVERSE_CC(ChunkId);

			sLogger.LogInfo(L"Loaded '%s' M2 chunk, size %u", ChunkIdToStr(ChunkId, false).c_str(), ChunkSize);
			switch (eChunk)
			{
				case EM2Chunk::Model: Chunk = new MD21Chunk(); break;
				case EM2Chunk::Physic: Chunk = new PFIDChunk(); break;
				case EM2Chunk::Animation: Chunk = new AFIDChunk(); break;
				case EM2Chunk::Bone: Chunk = new BFIDChunk(); break;
				case EM2Chunk::Skeleton: Chunk = new SKIDChunk(); break;
				case EM2Chunk::Skin: Chunk = new SFIDChunk(); break;
				case EM2Chunk::Texture: Chunk = new TXIDChunk(); break;
				case EM2Chunk::TXAC:
					PostProcessChunks[eChunk] = { (uint32_t)FileStream.tellg(), ChunkSize };
					FileStream.seekg(ChunkSize, std::ios::cur);
					continue;
				default:
					Chunk = new RawChunk();
					break;
			}

			uint32_t savePos = (uint32_t)FileStream.tellg();
			Chunk->Load(FileStream, ChunkSize);
			FileStream.seekg(savePos + ChunkSize, std::ios::beg);

			Chunks[eChunk] = Chunk;
		}
	}

	auto ModelChunk = (MD21Chunk*)GetChunk(EM2Chunk::Model);
	if (!ModelChunk)
	{
		sLogger.LogError(L"Error: '%s' chunk not found in model", ChunkIdToStr((uint32_t)EM2Chunk::Model, true).c_str());
		return EError_FailedToLoadM2_FileCorrupt;
	}

	m_OriginalModelChunkSize = ModelChunk->RawData.size();

	// load header
	memcpy(&Header, ModelChunk->RawData.data(), sizeof(Header));
	if (!Header.IsLongHeader() || GetExpansion() < Expansion::Cataclysm)
	{
		sLogger.LogInfo(L"Short header detected");
		Header.Elements.nTextureCombinerCombo = 0;
		Header.Elements.oTextureCombinerCombo = 0;
	}

	if (Header.Description.Version < 263 || Header.Description.Version > 274)
	{
		sLogger.LogError(L"Error: Unsupported model version %u", Header.Description.Version);
		return EError_FailedToLoadM2_VersionNotSupported;
	}

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(m_OriginalModelChunkSize);

	// load elements
	for (uint32_t i = 0; i < EElement__Count__; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load(ModelChunk->RawData.data(), 0))
		{
			sLogger.LogError(L"Error: Failed to load M2 element #%u", i);
			return EError_FailedToLoadM2_FileCorrupt;
		}
	}

	// load skins
	if ((Header.Elements.nSkin == 0) || (Header.Elements.nSkin > SKIN_COUNT - LOD_SKIN_MAX_COUNT))
	{
		sLogger.LogError(L"Error: Unsupported number of skins in model: %u", Header.Elements.nSkin);
		return EError_FailedToLoadM2_FileCorrupt;
	}

	if (auto chunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin))
	{
		sLogger.LogInfo(L"Used skin files:");
		for (auto fileDataId : chunk->SkinsFileDataIds)
			sLogger.LogInfo(L"\t[%u] %s", fileDataId, storageRef->PathInfo(fileDataId));
	}

	auto Error = LoadSkins();
	if (Error != EError::EError_OK)
		return Error;

	hasLodSkins = false;
	auto skinChunk = (M2Chunk::SFIDChunk*)GetChunk(EM2Chunk::Skin);
	if (skinChunk && skinChunk->SkinsFileDataIds.size() > Header.Elements.nSkin)
		hasLodSkins = true;
	else
	{
		for (int i = 0; i < LOD_SKIN_MAX_COUNT; ++i)
		{
			std::wstring FileNameSkin;
			if (!GetFileSkin(FileNameSkin, _FileName, SKIN_COUNT - LOD_SKIN_MAX_COUNT + i, false))
				continue;

			sLogger.LogInfo(L"Loading skin '%s'...", FileNameSkin);
			M2Skin LoDSkin(this);
			if (EError Error = LoDSkin.Load(FileNameSkin.c_str()))
			{
				if (Error == EError_FailedToLoadSKIN_CouldNotOpenFile)
					continue;

				sLogger.LogError(L"Error: Failed to load #%u lod skin %s", i, FileNameSkin);
				return Error;
			}
			else
			{
				hasLodSkins = true;
				break;
			}
		}
	}

	for (auto& PostChunk : PostProcessChunks)
	{
		FileStream.seekg(PostChunk.second.Offs, std::ios::beg);
		ChunkBase* Chunk = NULL;
		switch (PostChunk.first)
		{
			case EM2Chunk::TXAC: Chunk = new TXACChunk(Header.Elements.nTextureFlags, Header.Elements.nParticleEmitter); break;
			default: Chunk = new RawChunk(); break;
		}

		Chunk->Load(FileStream, PostChunk.second.Size);
		Chunks[PostChunk.first] = Chunk;
	}

	if (auto chunk = (SKIDChunk*)GetChunk(EM2Chunk::Skeleton))
	{
		sLogger.LogInfo(L"Used skeleton file:");
		sLogger.LogInfo(L"\t[%u] %s", chunk->SkeletonFileDataId, storageRef->PathInfo(chunk->SkeletonFileDataId));
	}

	Error = LoadSkeleton();
	if (Error != EError_OK)
		return Error;

	// print info
	//PrintInfo();
	PrintReferencedFileInfo();

	sLogger.LogInfo(L"Finished loading M2");

	//m_SaveElements_FindOffsets();

	// done
	return EError_OK;
}

M2Lib::EError M2Lib::M2::SetReplaceM2(const wchar_t* FileName)
{
	auto replaceM2 = new M2();
	auto Error = replaceM2->Load(FileName);
	if (Error != EError_OK)
		return Error;

	this->replaceM2 = replaceM2;
	return EError_OK;
}

M2Lib::EError M2Lib::M2::LoadSkeleton()
{
	std::wstring FileNameSkeleton;
	if (!GetFileSkeleton(FileNameSkeleton, _FileName, false))
		return EError_OK;

	if (!std::filesystem::exists(FileNameSkeleton))
	{
		sLogger.LogError(L"Error: Skeleton file %s not found!", FileNameSkeleton);
		return EError_FailedToLoadSkeleton_CouldNotOpenFile;
	}

	auto sk = new M2Lib::Skeleton();
	auto loadResult = sk->Load(FileNameSkeleton.c_str());
	if (loadResult != EError_OK)
	{
		if (GetChunk(EM2Chunk::Skeleton))
			sLogger.LogError(L"Error: model has skeleton chunk, but chunk file not loaded!");
		delete sk;

		return loadResult;
	}

	sLogger.LogInfo(L"Sekeleton file detected and loaded");
	Skeleton = sk;

	if (auto chunk = (SkeletonChunk::SKPDChunk*)Skeleton->GetChunk(SkeletonChunk::ESkeletonChunk::SKPD))
	{
		if (auto info = storageRef->GetFileInfoByFileDataId(chunk->Data.ParentSkeletonFileId))
		{
			std::filesystem::path ParentSkeletonPath;
			if (wcslen(Settings.WorkingDirectory))
				ParentSkeletonPath = std::filesystem::path(Settings.WorkingDirectory) / info->Path;
			else
				ParentSkeletonPath = std::filesystem::path(_FileName).parent_path() / std::filesystem::path(info->Path).filename();

			auto parentSkeleton = new M2Lib::Skeleton();
			auto Error = parentSkeleton->Load(ParentSkeletonPath.c_str());
			if (Error == EError_OK)
			{
				sLogger.LogInfo(L"Parent skeleton file [%u] %s loaded", chunk->Data.ParentSkeletonFileId, ParentSkeletonPath.wstring().c_str());
				ParentSkeleton = parentSkeleton;
			}
			else
			{
				sLogger.LogError(L"Error: Failed to load parent skeleton file [%u] %s", chunk->Data.ParentSkeletonFileId, ParentSkeletonPath.wstring().c_str());
				delete parentSkeleton;
				return EError_FailedToLoadSkeleton_CouldNotOpenFile;
			}
		}
		else
		{
			sLogger.LogError(L"Error: skeleton has parent skeleton chunk, but parent file not loaded!");
			return EError_FailedToLoadSkeleton_CouldNotOpenFile;
		}
	}

	return EError_OK;
}

M2Lib::EError M2Lib::M2::SaveSkeleton(std::wstring const& M2FileName)
{
	if (!Skeleton)
		return EError_OK;

	std::wstring SkeletonFileName;
	if (!GetFileSkeleton(SkeletonFileName, M2FileName, true))
		return EError_OK;

	auto Error = Skeleton->Save(SkeletonFileName.c_str());
	if (Error != EError_OK)
		return Error;

	if (!ParentSkeleton)
		return EError_OK;

	std::wstring ParentSkeletonFileName;
	if (!GetFileParentSkeleton(ParentSkeletonFileName, M2FileName, true))
	{
		sLogger.LogError(L"Error: Failed to save parent skeleton - cannot determine output file");
		return EError_FailedToSaveM2_NoFileSpecified;
	}

	Error = ParentSkeleton->Save(ParentSkeletonFileName.c_str());
	if (Error != EError_OK)
		return Error;

	return EError_OK;
}

M2Lib::EError M2Lib::M2::LoadSkins()
{
	for (uint32_t i = 0; i < Header.Elements.nSkin; ++i)
	{
		std::wstring FileNameSkin;
		if (!GetFileSkin(FileNameSkin, _FileName, i, false))
			continue;

		Skins[i] = new M2Skin(this);
		if (EError Error = Skins[i]->Load(FileNameSkin.c_str()))
		{
			delete Skins[i];
			Skins[i] = NULL;
			return Error;
		}
	}

	return EError::EError_OK;
}

M2Lib::EError M2Lib::M2::SaveSkins(wchar_t const* M2FileName)
{
	if (Header.Elements.nSkin == 0 || Header.Elements.nSkin > SKIN_COUNT - LOD_SKIN_MAX_COUNT)
		return EError_FailedToSaveM2;

	// delete existing skin files
	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
	{
		std::wstring FileNameSkin;
		if (GetFileSkin(FileNameSkin, M2FileName, i, true))
			_wremove(FileNameSkin.c_str());
	}

	for (uint32_t i = 0; i < Header.Elements.nSkin; ++i)
	{
		std::wstring FileNameSkin;
		if (!GetFileSkin(FileNameSkin, M2FileName, i, true))
			continue;

		if (EError Error = Skins[i]->Save(FileNameSkin.c_str()))
			return Error;
	}

	auto skinChunk = (M2Chunk::SFIDChunk*)GetChunk(EM2Chunk::Skin);

	// 0x80 = flag_has_lod_skin_files - wrong
	//if (Header.Description.Flags & 0x80)
	uint32_t LodSkinCount = skinChunk ? skinChunk->SkinsFileDataIds.size() - Header.Elements.nSkin : (hasLodSkins ? LOD_SKIN_MAX_COUNT : 0);
	for (uint32_t i = 0; i < LodSkinCount; ++i)
	{
		std::wstring FileNameSkin;
		if (!GetFileSkin(FileNameSkin, M2FileName, i + 4, true))
			continue;

		if (EError Error = (Skins[1] ? Skins[1] : Skins[0])->Save(FileNameSkin.c_str()))
			return Error;
	}

	return EError_OK;
}

void M2Lib::M2::DoExtraWork()
{
	//auto RenderFlags = Elements[EElement_TextureFlags].as<CElement_TextureFlag>();
	//sLogger.LogInfo("Existing render flags:");
	//for (uint32_t i = 0; i < Elements[EElement_TextureFlags].Count; ++i)
	//	sLogger.LogInfo("\tFlags: %u Blend: %u", RenderFlags[i].Flags, RenderFlags[i].Blend);

	std::map<std::pair<int16_t, int16_t>, uint32_t> renderFlagsByStyle;

	M2SkinElement::TextureLookupRemap textureRemap;
	// first assign shader data
	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
	{
		auto Skin = Skins[i];
		if (!Skin)
			continue;

		auto Materials = Skin->Elements[M2SkinElement::EElement_Material].as<M2SkinElement::CElement_Material>();

		for (uint32_t MeshIndex = 0; MeshIndex < Skin->ExtraDataBySubmeshIndex.size(); ++MeshIndex)
		{
			auto ExtraData = Skin->ExtraDataBySubmeshIndex[MeshIndex];

			// copy materials will be done later
			if (ExtraData->MaterialOverride >= 0)
				continue;

			auto ShaderId = ExtraData->ShaderId;
			if (ShaderId == -1 && ExtraData->TextureType[0] != -1)
				ShaderId = TRANSPARENT_SHADER_ID;

			if (ExtraData->BlendMode != -1)
			{
				auto key = std::pair<int16_t, int16_t>(ExtraData->RenderFlags, ExtraData->BlendMode);
				if (renderFlagsByStyle.find(key) == renderFlagsByStyle.end())
					renderFlagsByStyle[key] = AddTextureFlags((CElement_TextureFlag::EFlags)ExtraData->RenderFlags, (CElement_TextureFlag::EBlend)ExtraData->BlendMode);

				for (uint32_t j = 0; j < Skin->Header.nMaterial; ++j)
				{
					if (Materials[j].iSubMesh != MeshIndex)
						continue;

					Materials[j].iRenderFlags = renderFlagsByStyle[key];
				}
			}

			if (ShaderId == -1)
				continue;

			if (!Skin->AddShader(ShaderId, ExtraData->TextureType, ExtraData->TextureName, { MeshIndex }, textureRemap))
				break;
		}
	}

	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
	{
		auto Skin = Skins[i];
		if (!Skin)
			continue;

		// copy materials
		for (uint32_t MeshIndex = 0; MeshIndex < Skin->ExtraDataBySubmeshIndex.size(); ++MeshIndex)
		{
			auto ExtraData = Skin->ExtraDataBySubmeshIndex[MeshIndex];
			if (ExtraData->MaterialOverride < 0)
				continue;

			if (i == 0)
				Skin->CopyMaterial(ExtraData->MaterialOverride, MeshIndex);
			else
			{
				// translate index from zero skin to current skin
				for (uint32_t LocalMeshIndex = 0; LocalMeshIndex < Skin->ExtraDataBySubmeshIndex.size(); ++LocalMeshIndex)
				{
					auto LocalExtraData = Skin->ExtraDataBySubmeshIndex[LocalMeshIndex];
					if (LocalExtraData->FirstLODMeshIndex == ExtraData->MaterialOverride)
					{
						Skin->CopyMaterial(LocalMeshIndex, MeshIndex);
						break;
					}
				}
			}
		}
	}

	if (needRemoveTXIDChunk)
		RemoveTXIDChunk();
}

M2Lib::ChunkBase* M2Lib::M2::GetChunk(EM2Chunk ChunkId)
{
	auto chunkItr = Chunks.find(ChunkId);
	if (chunkItr == Chunks.end())
		return NULL;

	return chunkItr->second;
}

void M2Lib::M2::RemoveChunk(EM2Chunk ChunkId)
{
	auto chunkItr = Chunks.find(ChunkId);
	if (chunkItr != Chunks.end())
		Chunks.erase(chunkItr);
}

void M2Lib::M2::CopyReplaceChunks()
{
	// TODO: leave only non-lod filedataids in skin chunk?
	if (!replaceM2)
		return;

	auto skinChunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin);
	auto otherSkinChunk = (SFIDChunk*)replaceM2->GetChunk(EM2Chunk::Skin);

	if (!otherSkinChunk)
	{
		if (skinChunk)
		{
			sLogger.LogInfo(L"Model to replace does not have SKIN chunk, removing source one...");
			RemoveChunk(EM2Chunk::Skin);
			delete skinChunk;
		}
	}
	else
	{
		sLogger.LogInfo(L"Copying skin chunk to source model...");
		if (skinChunk)
			delete skinChunk;
		Chunks[EM2Chunk::Skin] = otherSkinChunk;
		Header.Elements.nSkin = replaceM2->Header.Elements.nSkin;
		replaceM2->RemoveChunk(EM2Chunk::Skin);
	}

	auto otherSkeletonChunk = replaceM2->GetChunk(EM2Chunk::Skeleton);
	auto skeletonChunk = GetChunk(EM2Chunk::Skeleton);

	if (!otherSkeletonChunk)
	{
		if (skeletonChunk)
			sLogger.LogWarning(L"Warning: replaced model does not have skeleton file, but source does. Model will use source skeleton file which may cause explosions");
	}
	else
	{
		sLogger.LogInfo(L"Copying skeleton chunk to source model...");
		if (skeletonChunk)
			delete skeletonChunk;

		Chunks[EM2Chunk::Skeleton] = otherSkeletonChunk;
		replaceM2->RemoveChunk(EM2Chunk::Skeleton);
	}
}

void M2Lib::M2::FixSkinChunk()
{
	auto skinChunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin);
	if (!skinChunk)
		return;

	int32_t SkinDiff = OriginalSkinCount - Header.Elements.nSkin;
	if (SkinDiff > 0)
	{
		sLogger.LogInfo(L"There was %u more skins in original M2, removing extra from skin chunk", SkinDiff);
		for (uint32_t i = 0; i < (uint32_t)SkinDiff; ++i)
			skinChunk->SkinsFileDataIds.erase(skinChunk->SkinsFileDataIds.begin() + Header.Elements.nSkin);
	}
	else if (SkinDiff < 0)
	{
		sLogger.LogInfo(L"Generated model has %u more skins than needed, removing extra", -SkinDiff);
		Header.Elements.nSkin = replaceM2 ? replaceM2->Header.Elements.nSkin : OriginalSkinCount;
	}
}

M2Lib::EError M2Lib::M2::Save(const wchar_t* FileName)
{
	// check path
	if (!FileName)
		return EError_FailedToSaveM2_NoFileSpecified;

	auto directory = std::filesystem::path(FileName).parent_path();
	if (!std::filesystem::is_directory(directory) && !std::filesystem::create_directories(directory))
	{
		sLogger.LogError(L"Failed to write to directory '%s'", directory.wstring().c_str());

		return EError_FailedToSaveM2;
	}

	if (replaceM2)
		this->replaceM2 = replaceM2;

	DoExtraWork();

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToSaveM2;

	sLogger.LogInfo(L"Saving model to %s", FileName);

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// Reserve model chunk header
	uint32_t const ChunkReserveOffset = 8;

	uint32_t ChunkId = REVERSE_CC((uint32_t)EM2Chunk::Model);
	FileStream.write((char*)&ChunkId, 4);
	FileStream.seekp(4, std::ios::cur);		// reserve bytes for chunk size

	//Header.Description.Version = 0x0110;
	//Header.Description.Flags &= ~0x80;

	// save header
	uint32_t HeaderSize = GetHeaderSize();
	FileStream.write((char*)&Header, HeaderSize);

	// save elements
	uint32_t ElementCount = Header.IsLongHeader() ? EElement__Count__ : EElement__Count__ - 1;
	for (uint32_t i = 0; i < ElementCount; ++i)
	{
		if (!Elements[i].Save(FileStream, ChunkReserveOffset))
			return EError_FailedToSaveM2;
	}

	uint32_t MD20Size = (uint32_t)FileStream.tellp();
	MD20Size -= ChunkReserveOffset;

	FileStream.seekp(4, std::ios::beg);
	FileStream.write((char*)(&MD20Size), 4);

	FileStream.seekp(0, std::ios::end);

	FixSkinChunk();

	for (auto chunk : Chunks)
	{
		if (chunk.first == EM2Chunk::Model)
			continue;

		//if (chunk.first == 'SFID')
		//	continue;

		uint32_t ChunkId = REVERSE_CC((uint32_t)chunk.first);

		FileStream.write((char*)&ChunkId, 4);
		FileStream.seekp(4, std::ios::cur);		// reserve space for chunk size
		uint32_t savePos = (uint32_t)FileStream.tellp();
		chunk.second->Save(FileStream);
		uint32_t ChunkSize = (uint32_t)FileStream.tellp() - savePos;
		FileStream.seekp(savePos - 4, std::ios::beg);
		FileStream.write((char*)&ChunkSize, 4);
		FileStream.seekp(0, std::ios::end);
	}

	// save skins
	auto Error = SaveSkins(FileName);
	if (Error != EError_OK)
		return Error;

	Error = SaveSkeleton(FileName);
	if (Error != EError_OK)
		return Error;

	if (auto chunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin))
	{
		sLogger.LogInfo(L"INFO: Put your skins to:");
		for (auto fileDataId : chunk->SkinsFileDataIds) {
			sLogger.LogInfo(L"\t%s", storageRef->PathInfo(fileDataId));
			}
	}

	if (auto chunk = (SKIDChunk*)GetChunk(EM2Chunk::Skeleton))
	{
		sLogger.LogInfo(L"INFO: Put your skeleton to:");
		sLogger.LogInfo(L"\t%s", storageRef->PathInfo(chunk->SkeletonFileDataId));
	}

	return EError_OK;
}

M2Lib::EError M2Lib::M2::ExportM2Intermediate(wchar_t const* FileName)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToExportM2I_CouldNotOpenFile;

	// open binary stream
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// save signature
	DataBinary.WriteFourCC(M2I::Signature_M2I0);

	// save version
	DataBinary.Write<uint16_t>(8);
	DataBinary.Write<uint16_t>(1);

	// get data to save
	M2Skin* pSkin = Skins[0];

	uint32_t SubsetCount = pSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* Subsets = pSkin->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	CVertex* Vertices = Elements[EElement_Vertex].as<CVertex>();
	auto verticesCount = Elements[EElement_Vertex].Count;
	uint16_t* Triangles = pSkin->Elements[M2SkinElement::EElement_TriangleIndex].as<uint16_t>();
	auto trianglesCount = pSkin->Elements[M2SkinElement::EElement_TriangleIndex];
	uint16_t* Indices = pSkin->Elements[M2SkinElement::EElement_VertexLookup].as<uint16_t>();
	auto indicesCount = pSkin->Elements[M2SkinElement::EElement_VertexLookup].Count;

	// save subsets
	DataBinary.Write<uint32_t>(SubsetCount);
	for (uint32_t i = 0; i < SubsetCount; ++i)
	{
		M2SkinElement::CElement_SubMesh* pSubsetOut = &Subsets[i];

		DataBinary.Write<uint16_t>(pSubsetOut->ID);	// mesh id
		DataBinary.WriteASCIIString("");		// description
		DataBinary.Write<int16_t>(-1);				// material override

		DataBinary.Write<int32_t>(-1);				// shader id
		DataBinary.Write<int16_t>(-1);				// blend type
		DataBinary.Write<uint16_t>(0);				// render flags
		
		for (uint32_t j = 0; j < MAX_SUBMESH_TEXTURES; ++j)
		{
			DataBinary.Write<uint16_t>(-1);			// texture type
			DataBinary.WriteASCIIString("");	// texture
		}

		DataBinary.Write<uint32_t>(i);				// original subset index

		DataBinary.Write<uint16_t>(pSubsetOut->Level);

		// write vertices
		DataBinary.Write<uint32_t>(pSubsetOut->VertexCount);
		uint32_t VertexEnd = pSubsetOut->VertexStart + pSubsetOut->VertexCount;
		for (uint32_t k = pSubsetOut->VertexStart; k < VertexEnd; ++k)
		{
			m2lib_assert(k < indicesCount && "Skin references vertex index that is outside bounds of indices array");
			m2lib_assert(Indices[k] < verticesCount && "Skin references vertex that is outside bounds of vertex array");

			CVertex const& Vertex = Vertices[Indices[k]];

			DataBinary.WriteC3Vector(Vertex.Position);

			for (uint32_t j = 0; j < BONES_PER_VERTEX; ++j)
				DataBinary.Write<uint8_t>(Vertex.BoneWeights[j]);

			for (uint32_t j = 0; j < BONES_PER_VERTEX; ++j)
				DataBinary.Write<uint8_t>(Vertex.BoneIndices[j]);

			DataBinary.WriteC3Vector(Vertex.Normal);

			for (uint32_t j = 0; j < MAX_SUBMESH_UV; ++j)
				DataBinary.WriteC2Vector(Vertex.Texture[j]);
		}

		// write triangles
		uint32_t SubsetTriangleCountOut = pSubsetOut->TriangleIndexCount / 3;
		DataBinary.Write<uint32_t>(SubsetTriangleCountOut);

		uint32_t TriangleIndexStart = pSubsetOut->GetStartTrianlgeIndex();
		uint32_t TriangleIndexEnd = pSubsetOut->GetEndTriangleIndex();
		for (uint32_t k = TriangleIndexStart; k < TriangleIndexEnd; ++k)
		{
			uint16_t TriangleIndexOut = Triangles[k] - pSubsetOut->VertexStart;
			m2lib_assert(TriangleIndexOut < pSubsetOut->VertexCount);
			DataBinary.Write<uint16_t>(TriangleIndexOut);
		}
	}

	auto boneElement = GetBones();

	// write bones
	DataBinary.Write<uint32_t>(boneElement->Count);
	for (uint16_t i = 0; i < boneElement->Count; i++)
	{
		CElement_Bone& Bone = *boneElement->at<CElement_Bone>(i);

		DataBinary.Write<uint16_t>(i);
		DataBinary.Write<int16_t>(Bone.ParentBone);
		DataBinary.WriteC3Vector(Bone.Position);
		DataBinary.Write<uint8_t>(1);	// has data
		DataBinary.Write<uint32_t>(Bone.Flags);
		DataBinary.Write<uint16_t>(Bone.SubmeshId);
		DataBinary.Write<uint16_t>(Bone.Unknown[0]);
		DataBinary.Write<uint16_t>(Bone.Unknown[1]);
	}

	auto attachmentElement = GetAttachments();
	// write attachments
	DataBinary.Write<uint32_t>(attachmentElement->Count);
	for (uint16_t i = 0; i < attachmentElement->Count; i++)
	{
		CElement_Attachment& Attachment = *attachmentElement->at<CElement_Attachment>(i);

		DataBinary.Write<uint32_t>(Attachment.ID);
		DataBinary.Write<int16_t>(Attachment.ParentBone);
		DataBinary.WriteC3Vector(Attachment.Position);
		DataBinary.Write<float>(1.0f);
	}

	uint32_t CamerasCount = Elements[EElement_Camera].Count;
	// write cameras
	DataBinary.Write<uint32_t>(CamerasCount);
	for (uint16_t i = 0; i < CamerasCount; i++)
	{
		int32_t CameraType;
		float ClipFar, ClipNear, FoV;
		C3Vector Position, Target;

		if (GetExpansion() < Expansion::Cataclysm)
		{
			auto Camera = Elements[EElement_Camera].at<CElement_Camera_PreCata>(i);
			CameraType = Camera->Type;
			ClipFar = Camera->ClipFar;
			ClipNear = Camera->ClipNear;
			Position = Camera->Position;
			Target = Camera->Target;
			FoV = Camera->FoV;
		}
		else
		{
			auto Camera = Elements[EElement_Camera].at<CElement_Camera>(i);
			CameraType = Camera->Type;
			ClipFar = Camera->ClipFar;
			ClipNear = Camera->ClipNear;
			Position = Camera->Position;
			Target = Camera->Target;

			// extract field of view of camera from animation block
			if (Camera->AnimationBlock_FieldOfView.Keys.Count > 0)
			{
				auto ExternalAnimations = (M2Array*)Elements[EElement_Camera].GetLocalPointer(Camera->AnimationBlock_FieldOfView.Keys.Offset);
				auto LastElementIndex = GetLastElementIndex();
				m2lib_assert(LastElementIndex != M2Element::EElement__Count__);
				auto& LastElement = Elements[LastElementIndex];
				m2lib_assert(ExternalAnimations[0].Offset >= LastElement.Offset && ExternalAnimations[0].Offset < LastElement.Offset + LastElement.Data.size());

				float* FieldOfView_Keys = (float*)LastElement.GetLocalPointer(ExternalAnimations[0].Offset);
				FoV = FieldOfView_Keys[0];
			}
			else
				FoV = 0.785398163f;	// 45 degrees in radians, assuming that WoW stores camera FoV in radians. or maybe it's half FoV.
		}

		DataBinary.Write<uint8_t>(1);	// has data
		DataBinary.Write<int32_t>(CameraType);

		DataBinary.Write<float>(FoV);

		DataBinary.Write<float>(ClipFar);
		DataBinary.Write<float>(ClipNear);
		DataBinary.WriteC3Vector(Position);
		DataBinary.WriteC3Vector(Target);
	}

	FileStream.close();

	return EError_OK;
}

M2Lib::EError M2Lib::M2::ImportM2Intermediate(wchar_t const* FileName)
{
	if (!FileName)
		return EError_FailedToImportM2I_NoFileSpecified;

	// check that we have an M2 already loaded and ready to be injected
	if (!Header.Elements.nSkin)
		return EError_FailedToExportM2I_M2NotLoaded;

	if (pInM2I)
		delete pInM2I;
	pInM2I = new M2I();

	CopyReplaceChunks();

	auto Error = pInM2I->Load(FileName, this, !Settings.MergeBones, !Settings.MergeAttachments, !Settings.MergeCameras, Settings.IgnoreOriginalMeshIndexes);
	if (Error != EError_OK)
		return Error;

	// copy new vertex list from M2I to M2
	auto& NewVertexList = pInM2I->VertexList;
	Elements[EElement_Vertex].SetDataSize(NewVertexList.size(), NewVertexList.size() * sizeof(CVertex), false);
	memcpy(Elements[EElement_Vertex].Data.data(), &NewVertexList[0], NewVertexList.size() * sizeof(CVertex));

	// Disable for now
	//BoundaryData GlobalBoundary;
	//GlobalBoundary.Calculate(NewVertexList);
	//SetGlobalBoundingData(GlobalBoundary);

	// fix seams
	// this is hacky, but we gotta fix seams first
	// build skin 0
	// only build skin 0 for now, so we can fix seams and then build skin for real later
	M2SkinBuilder SkinBuilder;
	M2Skin* pNewSkin0 = new M2Skin(this);
	m2lib_assert(SkinBuilder.Build(pNewSkin0, 256, pInM2I, &NewVertexList[0], 0));

	// set skin 0 so we can begin seam fixing
	M2Skin* pOriginalSkin0 = Skins[0];	// save this because we will need to copy materials from it later.
	OriginalSkinCount = Header.Elements.nSkin;
	Header.Elements.nSkin = 1;
	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
	{
		if (Skins[i])
		{
			if (i != 0)
				delete Skins[i];
			Skins[i] = NULL;
		}
	}

	Skins[0] = pNewSkin0;

	if (Settings.FixSeams)
	{
		// fix normals within submeshes
		FixSeamsSubMesh(SubmeshPositionalTolerance, SubmeshAngularTolerance * DegreesToRadians);

		// fix normals between body submeshes
		FixSeamsBody(BodyPositionalTolerance, BodyAngularTolerance * DegreesToRadians);

		// close gaps between clothes and body
		FixSeamsClothing(ClothingPositionalTolerance, ClothingAngularTolerance * DegreesToRadians);
	}
	else if (Settings.FixEdgeNormals)
	{
		// fix normals on edges between meshes
		FixNormals(NormalAngularTolerance * DegreesToRadians);
	}

	//
	//
	//
	// build skins for real this time
	// because a few bone indices might have changed during seam and gap fixing
	// this list will store the new skins
	M2Skin* NewSkinList[SKIN_COUNT];
	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
		NewSkinList[i] = NULL;

	// for easy looping
	uint32_t MaxBoneList[SKIN_COUNT - LOD_SKIN_MAX_COUNT + 1];
	MaxBoneList[0] = 256;
	MaxBoneList[1] = 64;
	MaxBoneList[2] = 53;
	MaxBoneList[3] = 21;
	MaxBoneList[4] = 0;		// extra entry needed for LoD check
	//MaxBoneList[4] = 64;	// extracted from client
	//MaxBoneList[5] = 64;
	//MaxBoneList[6] = 64;

	std::vector<uint16_t> NewBoneLookup;
	int32_t BoneStart = 0;
	uint32_t iSkin = 0;

	for (uint32_t iLoD = 0; iLoD < SKIN_COUNT - LOD_SKIN_MAX_COUNT; ++iLoD)
	{
		M2Skin* pNewSkin = new M2Skin(this);
		m2lib_assert(SkinBuilder.Build(pNewSkin, MaxBoneList[iLoD], pInM2I, Elements[EElement_Vertex].as<CVertex>(), BoneStart));
		if (iLoD == 0)
		{
			// fill extra data with mesh indexes from zero skin
			for (uint32_t i = 0; i < pNewSkin->ExtraDataBySubmeshIndex.size(); ++i)
				const_cast<SubmeshExtraData*>(pNewSkin->ExtraDataBySubmeshIndex[i])->FirstLODMeshIndex = i;
		}

		// if there are more bones than the next lowest level of detail
		if (SkinBuilder.m_Bones.size() > MaxBoneList[iLoD + 1])
		{
			// copy skin to result list
			NewSkinList[iSkin++] = pNewSkin;

			// copy skin's bone lookup to the global bone lookup list
			for (uint32_t i = 0; i < SkinBuilder.m_Bones.size(); i++)
				NewBoneLookup.push_back(SkinBuilder.m_Bones[i]);

			// advance for where next skin's bone lookup will begin
			BoneStart += SkinBuilder.m_Bones.size();
		}
		else
		{
			// this skin does not have enough bones and so it is not needed because the next lowest level of detail can contain the whole thing just fine, so discard this skin.
			delete pNewSkin;
		}
	}

	// set skin count
	Header.Elements.nSkin = iSkin;

	// copy materials from old sub meshes to new sub meshes
	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
	{
		if (NewSkinList[i])
		{
			NewSkinList[i]->CopyMaterials(pOriginalSkin0);
			//NewSkinList[i]->SortSubMeshes();
		}
	}
	delete pOriginalSkin0;

	// replace old skins with new
	for (uint32_t i = 0; i < SKIN_COUNT; ++i)
	{
		if (Skins[i])
			delete Skins[i];

		Skins[i] = NewSkinList[i];
	}

	// copy new bone lookup
	Elements[EElement_SkinnedBoneLookup].SetDataSize(NewBoneLookup.size(), NewBoneLookup.size() * sizeof(uint16_t), false);
	memcpy(Elements[EElement_SkinnedBoneLookup].Data.data(), &NewBoneLookup[0], NewBoneLookup.size() * sizeof(uint16_t));

	// build vertex bone indices
	for (uint32_t i = 0; i < Header.Elements.nSkin; ++i)
	{
		if (!Skins[i])
			continue;

		Skins[i]->BuildVertexBoneIndices();
		Skins[i]->m_SaveElements_FindOffsets();
		Skins[i]->m_SaveElements_CopyElementsToHeader();
	}

	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// done, ready to be saved
	return EError_OK;
}

void M2Lib::M2::SetGlobalBoundingData(BoundaryData& Data)
{
	auto ExtraData = Data.CalculateExtra();

	Elements[EElement_BoundingVertex].SetDataSize(BOUNDING_VERTEX_COUNT, sizeof(CElement_BoundingVertices) * BOUNDING_VERTEX_COUNT, false);
	auto boundingVertices = Elements[EElement_BoundingVertex].as<CElement_BoundingVertices>();
	for (uint32_t i = 0; i < BOUNDING_VERTEX_COUNT; ++i)
		boundingVertices[i].Position = ExtraData.BoundingVertices[i];

	Elements[EElement_BoundingNormal].SetDataSize(BOUNDING_TRIANGLE_COUNT, sizeof(CElement_BoundingNormals) * BOUNDING_TRIANGLE_COUNT, false);
	auto boundingNormals = Elements[EElement_BoundingNormal].as<CElement_BoundingNormals>();
	for (uint32_t i = 0; i < BOUNDING_TRIANGLE_COUNT; ++i)
		boundingNormals[i].Normal = ExtraData.BoundingNormals[i];

	Elements[EElement_BoundingTriangle].SetDataSize(BOUNDING_TRIANGLE_COUNT * 3, sizeof(CElement_BoundingTriangle) * BOUNDING_TRIANGLE_COUNT * 3, false);
	auto boundingTriangles = Elements[EElement_BoundingTriangle].as<CElement_BoundingTriangle>();
	for (uint32_t i = 0; i < BOUNDING_TRIANGLE_COUNT * 3; ++i)
		boundingTriangles[i].Index = BoundaryData::ExtraData::BoundingTriangleVertexMap[i];
}

void M2Lib::M2::PrintInfo()
{
	//
	//
	// just print out any sort of data that we want to analize when troubleshooting

	uint32_t Count = 0;

	std::wstring FileOut = std::filesystem::path(_FileName).replace_extension("txt");

	std::wfstream FileStream;
	FileStream.open(FileOut.c_str(), std::ios::out | std::ios::trunc);

	FileStream << "ID       " << Header.Description.ID[0] << Header.Description.ID[1] << Header.Description.ID[2] << Header.Description.ID[3] << std::endl;		// 'MD20'
	FileStream << "Version  " << Header.Description.Version << std::endl;
	FileStream << std::endl;

	FileStream << "nName                     " << Header.Description.nName << std::endl;
	FileStream << "oName                     " << Header.Description.oName << std::endl;
	FileStream << " Value                    " << Elements[EElement_Name].as<char>() << std::endl;
	FileStream << std::endl;

	FileStream << "Flags                     " << Header.Description.Flags.Raw << std::endl;
	FileStream << std::endl;

	FileStream << "nGlobalSequences          " << Header.Elements.nGlobalSequence << std::endl;
	FileStream << "oGlobalSequences          " << Header.Elements.oGlobalSequence << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_GlobalSequence].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAnimations               " << Header.Elements.nAnimation << std::endl;
	FileStream << "oAnimations               " << Header.Elements.oAnimation << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Animation].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAnimationsLookup         " << Header.Elements.nAnimationLookup << std::endl;
	FileStream << "oAnimationsLookup         " << Header.Elements.oAnimationLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_AnimationLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBones                    " << Header.Elements.nBone << std::endl;
	FileStream << "oBones                    " << Header.Elements.oBone << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Bone].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nKeyBoneLookup            " << Header.Elements.nKeyBoneLookup << std::endl;
	FileStream << "oKeyBoneLookup            " << Header.Elements.oKeyBoneLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_KeyBoneLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nVertices                 " << Header.Elements.nVertex << std::endl;
	FileStream << "oVertices                 " << Header.Elements.oVertex << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Vertex].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nViews                    " << Header.Elements.nSkin << std::endl;
	FileStream << std::endl;

	FileStream << "nColors                   " << Header.Elements.nColor << std::endl;
	FileStream << "oColors                   " << Header.Elements.oColor << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Color].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextures                 " << Header.Elements.nTexture << std::endl;
	FileStream << "oTextures                 " << Header.Elements.oTexture << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Texture].Data.size() << std::endl;
	FileStream << std::endl;

    for (uint32_t i = 0; i < Header.Elements.nTexture; ++i)
    {
		CElement_Texture* texture = Elements[EElement_Texture].as<CElement_Texture>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tFlags: " << (uint32_t)texture[i].Flags << std::endl;
        FileStream << "\tType: " << (uint32_t)texture[i].Type << std::endl;
        if (texture[i].TexturePath.Count > 1)
			FileStream << "\tPath: " << GetTexturePath(i) << std::endl;
        else
            FileStream << "\tPath: None" << std::endl;
        FileStream << std::endl;
    }

	FileStream << "nTransparencies           " << Header.Elements.nTransparency << std::endl;
	FileStream << "oTransparencies           " << Header.Elements.oTransparency << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Transparency].Data.size() << std::endl;

	CElement_Transparency* Transparencies = Elements[EElement_Transparency].as<CElement_Transparency>();
    for (uint32_t i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto transparency = Transparencies[i];
        FileStream << "\t" << i << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.InterpolationType << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.GlobalSequenceID << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Times.Count << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Times.Offset << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Keys.Count << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Keys.Offset << std::endl;

            /*
            EInterpolationType InterpolationType;
            int16_t GlobalSequenceID;
            uint32_t nTimes;
            uint32_t oTimes;
            uint32_t nKeys;
            uint32_t oKeys;
            */
    }
	FileStream << std::endl;

	FileStream << "nTextureAnimation         " << Header.Elements.nTextureAnimation << std::endl;
	FileStream << "oTextureAnimation         " << Header.Elements.nTextureAnimation << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureAnimation].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureReplace           " << Header.Elements.nTextureReplace << std::endl;
	FileStream << "oTextureReplace           " << Header.Elements.oTextureReplace << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureReplace].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureFlags             " << Header.Elements.nTextureFlags << std::endl;
	FileStream << "oTextureFlags             " << Header.Elements.oTextureFlags << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureFlags].Data.size() << std::endl;
	CElement_TextureFlag* TextureFlags = Elements[EElement_TextureFlags].as<CElement_TextureFlag>();
    for (uint32_t i = 0; i < Header.Elements.nTextureFlags; ++i)
    {
        auto flag = TextureFlags[i];
        FileStream << "\t-- " << i << std::endl;
        FileStream << "\t" << flag.Flags << std::endl;
        FileStream << "\t" << flag.Blend << std::endl;
    }
	FileStream << std::endl;

	FileStream << "nSkinnedBoneLookup        " << Header.Elements.nSkinnedBoneLookup << std::endl;
	FileStream << "oSkinnedBoneLookup        " << Header.Elements.oSkinnedBoneLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_SkinnedBoneLookup].Data.size() << std::endl;
/*    EElement_SkinnedBoneLookup* SkinnedBonesLookup = (CElement_TextuEElement_SkinnedBoneLookupreFlag*)Elements[EElement_SkinnedBoneLookup].Data;
    for (auto i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto flag = TextureFlags[i];
        FileStream << "\t-- " << i << std::endl;
        FileStream << "\t" << flag.Flags << std::endl;
        FileStream << "\t" << flag.Blend << std::endl;
    }*/
	FileStream << std::endl;

	FileStream << "nTexturesLookup           " << Header.Elements.nTextureLookup << std::endl;
	FileStream << "oTexturesLookup           " << Header.Elements.oTextureLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureLookup].Data.size() << std::endl;
	FileStream << std::endl;

    for (uint32_t i = 0; i < Header.Elements.nTexture; ++i)
    {
        CElement_TextureLookup* textureLookup = Elements[EElement_TextureLookup].as<CElement_TextureLookup>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tIndex: " << textureLookup[i].TextureIndex << std::endl;
        FileStream << std::endl;
    }

	FileStream << "nTextureUnitsLookup       " << Header.Elements.nTextureUnitLookup << std::endl;
	FileStream << "oTextureUnitsLookup       " << Header.Elements.oTextureUnitLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureUnitLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTransparenciesLookup     " << Header.Elements.nTransparencyLookup << std::endl;
	FileStream << "oTransparenciesLookup     " << Header.Elements.oTransparencyLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TransparencyLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureAnimationsLookup  " << Header.Elements.nTextureAnimationLookup << std::endl;
	FileStream << "oTextureAnimationsLookup  " << Header.Elements.oTextureAnimationLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureAnimationLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "Volumes " << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min.X << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min.Y << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min.Z << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max.X << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max.Y << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max.Z << std::endl;
	FileStream << Header.Elements.CollisionVolume.Radius << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min.X << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min.Y << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min.Z << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max.X << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max.Y << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max.Z << std::endl;
	FileStream << Header.Elements.BoundingVolume.Radius << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingTriangles        " << Header.Elements.nBoundingTriangle << std::endl;
	FileStream << "oBoundingTriangles        " << Header.Elements.oBoundingTriangle << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingTriangle].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingVertices         " << Header.Elements.nBoundingVertex << std::endl;
	FileStream << "oBoundingVertices         " << Header.Elements.oBoundingVertex << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingVertex].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingNormals          " << Header.Elements.nBoundingNormal << std::endl;
	FileStream << "oBoundingNormals          " << Header.Elements.oBoundingNormal << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingNormal].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAttachments              " << Header.Elements.nAttachment << std::endl;
	FileStream << "oAttachments              " << Header.Elements.oAttachment << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Attachment].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAttachmentsLookup        " << Header.Elements.nAttachmentLookup << std::endl;
	FileStream << "oAttachmentsLookup        " << Header.Elements.oAttachmentLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_AttachmentLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nEvents                   " << Header.Elements.nEvent << std::endl;
	FileStream << "oEvents                   " << Header.Elements.oEvent << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Event].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nLights                   " << Header.Elements.nLight << std::endl;
	FileStream << "oLights                   " << Header.Elements.oLight << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Light].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nCameras                  " << Header.Elements.nCamera << std::endl;
	FileStream << "oCameras                  " << Header.Elements.oCamera << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Camera].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nCamerasLookup            " << Header.Elements.nCameraLookup << std::endl;
	FileStream << "oCamerasLookup            " << Header.Elements.oCameraLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_CameraLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nRibbonEmitters           " << Header.Elements.nRibbonEmitter << std::endl;
	FileStream << "oRibbonEmitters           " << Header.Elements.oRibbonEmitter << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_RibbonEmitter].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nParticleEmitters         " << Header.Elements.nParticleEmitter << std::endl;
	FileStream << "oParticleEmitters         " << Header.Elements.oParticleEmitter << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_ParticleEmitter].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureCombinerCombo     " << Header.Elements.nTextureCombinerCombo << std::endl;
	FileStream << "oTextureCombinerCombo     " << Header.Elements.oTextureCombinerCombo << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureCombinerCombo].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream.close();
}

void M2Lib::M2::PrintReferencedFileInfo()
{
	sLogger.LogInfo(L"=====START REFERENCED FILE INFO=======");
	auto skinChunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin);
	if (!skinChunk)
		sLogger.LogInfo(L"Skins files are not referenced by M2");
	auto skeletonChunk = (SKIDChunk*)GetChunk(EM2Chunk::Skeleton);
	if (!skeletonChunk)
		sLogger.LogInfo(L"Skeleton is not referenced by M2");
	auto textureChunk = (TXIDChunk*)GetChunk(EM2Chunk::Texture);
	if (!textureChunk)
		sLogger.LogInfo(L"Texture files are not referenced by M2");

	if (skinChunk)
	{
		sLogger.LogInfo(L"Total skin files referenced: %u", skinChunk->SkinsFileDataIds.size());
		for (auto skinFileDataId : skinChunk->SkinsFileDataIds)
			if (skinFileDataId)
				sLogger.LogInfo(L"\t[%u] %s", skinFileDataId, storageRef->PathInfo(skinFileDataId));
	}
	if (skeletonChunk)
	{
		sLogger.LogInfo(L"Skeleton file referenced:");
		sLogger.LogInfo(L"\t[%u] %s", skeletonChunk->SkeletonFileDataId, storageRef->PathInfo(skeletonChunk->SkeletonFileDataId));
		if (Skeleton)
		{
			using namespace SkeletonChunk;

			if (auto skpdChunk = (SKPDChunk*)Skeleton->GetChunk(ESkeletonChunk::SKPD))
				sLogger.LogInfo(L"\tParent skeleton file: [%u] %s", skpdChunk->Data.ParentSkeletonFileId, storageRef->PathInfo(skpdChunk->Data.ParentSkeletonFileId));
			else
				sLogger.LogInfo(L"\tNo parent skeleton referenced");
			if (auto afidChunk = (AFIDChunk*)Skeleton->GetChunk(ESkeletonChunk::AFID))
			{
				sLogger.LogInfo(L"\tTotal skeleton animation files referenced: %u", afidChunk->AnimInfos.size());
				for (auto anim : afidChunk->AnimInfos)
					sLogger.LogInfo(L"\t\t[%u] %s", anim.FileId, storageRef->PathInfo(anim.FileId));
			}
			if (auto boneChunk = (BFIDChunk*)Skeleton->GetChunk(ESkeletonChunk::BFID))
			{
				sLogger.LogInfo(L"\tTotal skeleton bone files referenced: %u", boneChunk->BoneFileDataIds.size());
				for (auto bone : boneChunk->BoneFileDataIds)
					sLogger.LogInfo(L"\t\t[%u] %s", bone, storageRef->PathInfo(bone));
			}
		}
		else
			sLogger.LogInfo(L"\tError! Skeleton is referenced, but not loaded");
	}
	if (textureChunk)
	{
		uint32_t count = 0;
		for (auto textuteFileDataId : textureChunk->TextureFileDataIds)
			if (textuteFileDataId)
				++count;

		sLogger.LogInfo(L"Total texture referenced in chunk: %u", count);
		for (auto textuteFileDataId : textureChunk->TextureFileDataIds)
			if (textuteFileDataId)
				sLogger.LogInfo(L"\t[%u] %s", textuteFileDataId, storageRef->PathInfo(textuteFileDataId));

		if (textureChunk->TextureFileDataIds.size() != Elements[EElement_Texture].Count)
			sLogger.LogInfo(L"\tError: M2 texture block element count is not equal to chunk element count! (%u vs %u)", Elements[EElement_Texture].Count, textureChunk->TextureFileDataIds.size());
		else
		{
			CElement_Texture* TextureElements = Elements[EElement_Texture].as<CElement_Texture>();
			for (uint32_t i = 0; i < Elements[EElement_Texture].Count; ++i)
			{
				auto& textureElement = TextureElements[i];
				if (textureElement.Type != CElement_Texture::ETextureType::Final_Hardcoded)
					continue;

				char const* localTexturePath = NULL;
				if (textureElement.TexturePath.Offset)
					localTexturePath = (char const*)Elements[EElement_Texture].GetLocalPointer(textureElement.TexturePath.Offset);
				auto FileDataId = textureChunk->TextureFileDataIds[i];

				if (textureElement.TexturePath.Offset && FileDataId && textureElement.TexturePath.Count > 1)
				{
					auto storagePath = storageRef->PathInfo(FileDataId);
					sLogger.LogInfo(L"\tWarning: Texture #%u file is stored in both chunk and texture element.\r\n"
						"\t\tElement path: %s\r\n"
						"\t\tChunk path: [%u] %s", i, StringHelpers::StringToWString(localTexturePath).c_str(), FileDataId, storagePath);
				}
				else if (textureElement.TexturePath.Offset && textureElement.TexturePath.Count > 1)
					sLogger.LogInfo(L"\tWarning: texture #%u '%s' is referenced in texture element but not in chunk (legacy model?)", i, StringHelpers::StringToWString(localTexturePath).c_str());
				else if (!textureElement.TexturePath.Offset && !FileDataId)
					sLogger.LogInfo(L"\tError: texture #%u path must be present in either in element or chunk, but it is not", i);
			}
		}
	}
	else
	{
		auto& TextureElement = Elements[EElement_Texture];
		CElement_Texture* Textures = TextureElement.as<CElement_Texture>();

		int count = 0;

		for (uint32_t j = 0; j < TextureElement.Count; ++j)
		{
			if (!Textures[j].TexturePath.Offset)
				continue;

			auto path = (const char*)TextureElement.GetLocalPointer(Textures[j].TexturePath.Offset);
			if (strlen(path) <= 1)
				continue;
			++count;
		}

		sLogger.LogInfo(L"Total texture referenced inplace: %u", count);

		for (uint32_t j = 0; j < TextureElement.Count; ++j)
		{
			if (!Textures[j].TexturePath.Offset)
				continue;

			auto path = (const char*)TextureElement.GetLocalPointer(Textures[j].TexturePath.Offset);
			if (strlen(path) <= 1)
				continue;

			sLogger.LogInfo(L"\t%s", StringHelpers::StringToWString(path).c_str());
		}
	}

	if (auto physChunk = (PFIDChunk*)GetChunk(EM2Chunk::Physic))
	{
		sLogger.LogInfo(L"Physics file referenced:");
		sLogger.LogInfo(L"\t[%u] %s", physChunk->PhysFileId, storageRef->PathInfo(physChunk->PhysFileId));
	}
	if (auto afidChunk = (AFIDChunk*)GetChunk(EM2Chunk::Animation))
	{
		sLogger.LogInfo(L"Total animation files referenced: %u", afidChunk->AnimInfos.size());
		for (auto anim : afidChunk->AnimInfos)
			sLogger.LogInfo(L"\t[%u] %s", anim.FileId, storageRef->PathInfo(anim.FileId));
	}
	if (auto boneChunk = (BFIDChunk*)GetChunk(EM2Chunk::Bone))
	{
		sLogger.LogInfo(L"Total bone files referenced: [%u]", boneChunk->BoneFileDataIds.size());
		for (auto bone : boneChunk->BoneFileDataIds)
			sLogger.LogInfo(L"\t[%u] %s", bone, storageRef->PathInfo(bone));
	}

	sLogger.LogInfo(L"======END OF REFERENCED FILE INFO======");
}

// Gets the .skin file names.
bool M2Lib::M2::GetFileSkin(std::wstring& SkinFileNameResultBuffer, std::wstring const& M2FileName, uint32_t SkinIndex, bool Save)
{
	M2Chunk::SFIDChunk* skinChunk;
	if (Save && replaceM2)
		skinChunk = (M2Chunk::SFIDChunk*)replaceM2->GetChunk(EM2Chunk::Skin);
	else
		skinChunk =(M2Chunk::SFIDChunk*)GetChunk(EM2Chunk::Skin);

	if (skinChunk)
	{
		//sLogger.LogInfo("Skin chunk detected");
		int32_t chunkIndex = -1;
		if (SkinIndex < SKIN_COUNT - LOD_SKIN_MAX_COUNT)
		{
			if (SkinIndex < Header.Elements.nSkin)
				chunkIndex = SkinIndex;
		}
		else
			chunkIndex = SkinIndex + LOD_SKIN_MAX_COUNT + Header.Elements.nSkin - SKIN_COUNT;

		if (chunkIndex < 0 || (uint32_t)chunkIndex >= skinChunk->SkinsFileDataIds.size())
		{
			//sLogger.LogInfo("Skin #%u is not present in chunk", SkinIndex);
			return false;
		}

		auto skinFileDataId = skinChunk->SkinsFileDataIds[chunkIndex];
		if (auto info = storageRef->GetFileInfoByFileDataId(skinFileDataId))
		{
			//sLogger.LogInfo("Skin listfile entry: %s", path.c_str());

			if (!Save && wcslen(Settings.WorkingDirectory))
				SkinFileNameResultBuffer = std::filesystem::path(Settings.WorkingDirectory) / info->Path;
			else if (Save && wcslen(Settings.OutputDirectory))
				SkinFileNameResultBuffer = std::filesystem::path(Settings.OutputDirectory) / info->Path;
			else
				SkinFileNameResultBuffer = std::filesystem::path(M2FileName).parent_path() / std::filesystem::path(info->Path).filename();

			return true;
		}

		sLogger.LogWarning(L"Warning: skin FileDataId [%u] not found in listfile! Listfile is not up to date! Trying default skin name", skinFileDataId);
	}

	SkinFileNameResultBuffer.resize(1024);

	switch (SkinIndex)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			std::swprintf((wchar_t*)SkinFileNameResultBuffer.data(),
				SkinFileNameResultBuffer.size(),
				L"%s\\%s0%u.skin",
				std::filesystem::path(M2FileName).parent_path().c_str(),
				std::filesystem::path(M2FileName).stem().c_str(),
				SkinIndex);
			break;
		case 4:
		case 5:
		case 6:
			std::swprintf((wchar_t*)SkinFileNameResultBuffer.data(),
				SkinFileNameResultBuffer.size(),
				L"%s\\%s_LOD0%u.skin",
				std::filesystem::path(M2FileName).parent_path().c_str(),
				std::filesystem::path(M2FileName).stem().c_str(),
				SkinIndex - 3);
			break;
		default:
			return false;
	}

	return true;
}

bool M2Lib::M2::GetFileSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName, bool Save)
{
	auto chunk = (M2Chunk::SKIDChunk*)GetChunk(EM2Chunk::Skeleton);
	if (!chunk)
		return false;

	if (auto info = storageRef->GetFileInfoByFileDataId(chunk->SkeletonFileDataId))
	{
		if (!Save && wcslen(Settings.WorkingDirectory))
			SkeletonFileNameResultBuffer = std::filesystem::path(Settings.WorkingDirectory) / info->Path;
		else if (Save && wcslen(Settings.OutputDirectory))
			SkeletonFileNameResultBuffer = std::filesystem::path(Settings.OutputDirectory) / info->Path;
		else
		{
			sLogger.LogInfo(L"Skeleton listfile entry: %s", info->Path.c_str());
			SkeletonFileNameResultBuffer = std::filesystem::path(M2FileName).parent_path() / std::filesystem::path(info->Path).filename();
		}
		return true;
	}
	
	SkeletonFileNameResultBuffer.resize(1024);
	sLogger.LogWarning(L"Warning: skeleton FileDataId [%u] not found in listfile! Listfile is not up to date! Trying default skeleton name", chunk->SkeletonFileDataId);
	std::swprintf((wchar_t*)SkeletonFileNameResultBuffer.data(),
		SkeletonFileNameResultBuffer.size(),
		L"%s\\%s.skel",
		std::filesystem::path(M2FileName).parent_path().c_str(),
		std::filesystem::path(M2FileName).stem().c_str());

	return true;
}

bool M2Lib::M2::GetFileParentSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName, bool Save) const
{
	if (!Skeleton)
		return false;

	auto chunk = dynamic_cast<SkeletonChunk::SKPDChunk*>(Skeleton->GetChunk(SkeletonChunk::ESkeletonChunk::SKPD));
	if (!chunk)
		return false;

	auto info = storageRef->GetFileInfoByFileDataId(chunk->Data.ParentSkeletonFileId);
	if (!info)
	{
		sLogger.LogError(L"Can't determine parent skeleton [%u] file name for model. Parent skeleton will not be saved", chunk->Data.ParentSkeletonFileId);
		return false;
	}

	sLogger.LogInfo(L"Parent skeleton listfile entry: %s", info->Path.c_str());

	if (!Save && wcslen(Settings.WorkingDirectory))
		SkeletonFileNameResultBuffer = std::filesystem::path(Settings.WorkingDirectory) / info->Path;
	else if (Save && wcslen(Settings.OutputDirectory))
		SkeletonFileNameResultBuffer = std::filesystem::path(Settings.OutputDirectory) / info->Path;
	else
		SkeletonFileNameResultBuffer = std::filesystem::path(M2FileName).parent_path() / std::filesystem::path(info->Path).filename();

	return true;
}

void M2Lib::M2::m_LoadElements_CopyHeaderToElements()
{
	Elements[EElement_Name].Count = Header.Description.nName;
	Elements[EElement_Name].Offset = Header.Description.oName;

	Elements[EElement_GlobalSequence].Count = Header.Elements.nGlobalSequence;
	Elements[EElement_GlobalSequence].Offset = Header.Elements.oGlobalSequence;

	Elements[EElement_Animation].Count = Header.Elements.nAnimation;
	Elements[EElement_Animation].Offset = Header.Elements.oAnimation;

	Elements[EElement_AnimationLookup].Count = Header.Elements.nAnimationLookup;
	Elements[EElement_AnimationLookup].Offset = Header.Elements.oAnimationLookup;

	Elements[EElement_Bone].Count = Header.Elements.nBone;
	Elements[EElement_Bone].Offset = Header.Elements.oBone;

	Elements[EElement_KeyBoneLookup].Count = Header.Elements.nKeyBoneLookup;
	Elements[EElement_KeyBoneLookup].Offset = Header.Elements.oKeyBoneLookup;

	Elements[EElement_Vertex].Count = Header.Elements.nVertex;
	Elements[EElement_Vertex].Offset = Header.Elements.oVertex;

	Elements[EElement_Color].Count = Header.Elements.nColor;
	Elements[EElement_Color].Offset = Header.Elements.oColor;

	Elements[EElement_Texture].Count = Header.Elements.nTexture;
	Elements[EElement_Texture].Offset = Header.Elements.oTexture;

	Elements[EElement_Transparency].Count = Header.Elements.nTransparency;
	Elements[EElement_Transparency].Offset = Header.Elements.oTransparency;

	Elements[EElement_TextureAnimation].Count = Header.Elements.nTextureAnimation;
	Elements[EElement_TextureAnimation].Offset = Header.Elements.oTextureAnimation;

	Elements[EElement_TextureReplace].Count = Header.Elements.nTextureReplace;
	Elements[EElement_TextureReplace].Offset = Header.Elements.oTextureReplace;

	Elements[EElement_TextureFlags].Count = Header.Elements.nTextureFlags;
	Elements[EElement_TextureFlags].Offset = Header.Elements.oTextureFlags;

	Elements[EElement_SkinnedBoneLookup].Count = Header.Elements.nSkinnedBoneLookup;
	Elements[EElement_SkinnedBoneLookup].Offset = Header.Elements.oSkinnedBoneLookup;

	Elements[EElement_TextureLookup].Count = Header.Elements.nTextureLookup;
	Elements[EElement_TextureLookup].Offset = Header.Elements.oTextureLookup;

	Elements[EElement_TextureUnitLookup].Count = Header.Elements.nTextureUnitLookup;
	Elements[EElement_TextureUnitLookup].Offset = Header.Elements.oTextureUnitLookup;

	Elements[EElement_TransparencyLookup].Count = Header.Elements.nTransparencyLookup;
	Elements[EElement_TransparencyLookup].Offset = Header.Elements.oTransparencyLookup;

	Elements[EElement_TextureAnimationLookup].Count = Header.Elements.nTextureAnimationLookup;
	Elements[EElement_TextureAnimationLookup].Offset = Header.Elements.oTextureAnimationLookup;

	Elements[EElement_BoundingTriangle].Count = Header.Elements.nBoundingTriangle;
	Elements[EElement_BoundingTriangle].Offset = Header.Elements.oBoundingTriangle;

	Elements[EElement_BoundingVertex].Count = Header.Elements.nBoundingVertex;
	Elements[EElement_BoundingVertex].Offset = Header.Elements.oBoundingVertex;

	Elements[EElement_BoundingNormal].Count = Header.Elements.nBoundingNormal;
	Elements[EElement_BoundingNormal].Offset = Header.Elements.oBoundingNormal;

	Elements[EElement_Attachment].Count = Header.Elements.nAttachment;
	Elements[EElement_Attachment].Offset = Header.Elements.oAttachment;

	Elements[EElement_AttachmentLookup].Count = Header.Elements.nAttachmentLookup;
	Elements[EElement_AttachmentLookup].Offset = Header.Elements.oAttachmentLookup;

	Elements[EElement_Event].Count = Header.Elements.nEvent;
	Elements[EElement_Event].Offset = Header.Elements.oEvent;

	Elements[EElement_Light].Count = Header.Elements.nLight;
	Elements[EElement_Light].Offset = Header.Elements.oLight;

	Elements[EElement_Camera].Count = Header.Elements.nCamera;
	Elements[EElement_Camera].Offset = Header.Elements.oCamera;

	Elements[EElement_CameraLookup].Count = Header.Elements.nCameraLookup;
	Elements[EElement_CameraLookup].Offset = Header.Elements.oCameraLookup;

	Elements[EElement_RibbonEmitter].Count = Header.Elements.nRibbonEmitter;
	Elements[EElement_RibbonEmitter].Offset = Header.Elements.oRibbonEmitter;

	Elements[EElement_ParticleEmitter].Count = Header.Elements.nParticleEmitter;
	Elements[EElement_ParticleEmitter].Offset = Header.Elements.oParticleEmitter;

	if (Header.IsLongHeader() && GetExpansion() >= Expansion::Cataclysm)
	{
		Elements[EElement_TextureCombinerCombo].Count = Header.Elements.nTextureCombinerCombo;
		Elements[EElement_TextureCombinerCombo].Offset = Header.Elements.oTextureCombinerCombo;
	}
}


void M2Lib::M2::m_LoadElements_FindSizes(uint32_t ChunkSize)
{
	for (uint32_t i = 0; i < EElement__Count__; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		uint32_t NextOffset = ChunkSize;
		for (uint32_t j = 0; j < EElement__Count__; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		m2lib_assert(NextOffset >= Element.Offset && "M2 Elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}
}

#define IS_LOCAL_ELEMENT_OFFSET(offset) \
	(!offset || Elements[iElement].Offset <= offset && offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size())
#define VERIFY_OFFSET_LOCAL( offset ) \
	m2lib_assert(IS_LOCAL_ELEMENT_OFFSET(offset));
#define VERIFY_OFFSET_NOTLOCAL( offset ) \
	m2lib_assert( !offset || offset >= Elements[iElement].OffsetOriginal + Elements[iElement].Data.size() );

M2Lib::DataElement* M2Lib::M2::GetAnimations()
{
	using namespace SkeletonChunk;

	if (auto animationChunk = Skeleton ? (SKS1Chunk*)Skeleton->GetChunk(ESkeletonChunk::SKS1) : NULL)
		return &animationChunk->Elements[SKS1Chunk::EElement_Animation];
	if (auto animationChunk = ParentSkeleton ? (SKS1Chunk*)ParentSkeleton->GetChunk(ESkeletonChunk::SKS1) : NULL)
		return &animationChunk->Elements[SKS1Chunk::EElement_Animation];
		
	return &Elements[EElement_Animation];
}

M2Lib::DataElement* M2Lib::M2::GetAnimationsLookup()
{
	using namespace SkeletonChunk;

	if (auto animationChunk = Skeleton ? (SKS1Chunk*)Skeleton->GetChunk(ESkeletonChunk::SKS1) : NULL)
		return &animationChunk->Elements[SKS1Chunk::EElement_AnimationLookup];
	if (auto animationChunk = ParentSkeleton ? (SKS1Chunk*)ParentSkeleton->GetChunk(ESkeletonChunk::SKS1) : NULL)
		return &animationChunk->Elements[SKS1Chunk::EElement_AnimationLookup];

	return &Elements[EElement_Animation];
}

M2Lib::DataElement* M2Lib::M2::GetBones()
{
	using namespace SkeletonChunk;

	if (auto boneChunk = Skeleton ? (SKB1Chunk*)Skeleton->GetChunk(ESkeletonChunk::SKB1) : NULL)
		return &boneChunk->Elements[SKB1Chunk::EElement_Bone];
	if (auto boneChunk = ParentSkeleton ? (SKB1Chunk*)ParentSkeleton->GetChunk(ESkeletonChunk::SKB1) : NULL)
		return &boneChunk->Elements[SKB1Chunk::EElement_Bone];

	return &Elements[EElement_Bone];
}

M2Lib::DataElement* M2Lib::M2::GetBoneLookups()
{
	using namespace SkeletonChunk;

	if (auto boneChunk = Skeleton ? (SKB1Chunk*)Skeleton->GetChunk(ESkeletonChunk::SKB1) : NULL)
		return &boneChunk->Elements[SKB1Chunk::EElement_KeyBoneLookup];
	if (auto boneChunk = ParentSkeleton ? (SKB1Chunk*)ParentSkeleton->GetChunk(ESkeletonChunk::SKB1) : NULL)
		return &boneChunk->Elements[SKB1Chunk::EElement_KeyBoneLookup];
		
	return &Elements[EElement_KeyBoneLookup];
}

M2Lib::DataElement* M2Lib::M2::GetAttachments()
{
	using namespace SkeletonChunk;

	if (auto attachmentChunk = Skeleton ? (SKA1Chunk*)Skeleton->GetChunk(ESkeletonChunk::SKA1) : NULL)
		return &attachmentChunk->Elements[SKA1Chunk::EElement_Attachment];
	if (auto attachmentChunk = ParentSkeleton ? (SKA1Chunk*)ParentSkeleton->GetChunk(ESkeletonChunk::SKA1) : NULL)
		return &attachmentChunk->Elements[SKA1Chunk::EElement_Attachment];
		
	return &Elements[EElement_Attachment];
}

M2Lib::SkeletonChunk::AFIDChunk* M2Lib::M2::GetSkeletonAFIDChunk()
{
	using namespace SkeletonChunk;

	if (auto chunk = Skeleton ? Skeleton->GetChunk(ESkeletonChunk::AFID) : NULL)
		return (SkeletonChunk::AFIDChunk*)chunk;
	if (auto chunk = ParentSkeleton ? ParentSkeleton->GetChunk(ESkeletonChunk::AFID) : NULL)
		return (SkeletonChunk::AFIDChunk*)chunk;
	

	return NULL;
}

void M2Lib::M2::m_SaveElements_FindOffsets()
{
	// fix animation offsets and find element offsets
	int32_t CurrentOffset = 0;
	if (Header.IsLongHeader() && GetExpansion() >= Expansion::Cataclysm)
		CurrentOffset = sizeof(CM2Header) + 8;	// +8 to align data at 16 byte bounds
	else
		CurrentOffset = sizeof(CM2Header) - 8;	// -8 because last 2 UInt32s are not saved

	// totaldiff needed to fix animations that are in the end of a chunk
	int32_t totalDiff = -(int32_t)m_OriginalModelChunkSize + GetHeaderSize();
	for (uint32_t iElement = 0; iElement < EElement__Count__; ++iElement)
		totalDiff += Elements[iElement].Data.size();

	int32_t OffsetDelta = 0;
	for (uint32_t iElement = 0; iElement < EElement__Count__; ++iElement)
	{
		// if this element has data...
		if (Elements[iElement].Data.empty())
		{
			Elements[iElement].Offset = 0;
			continue;
		}

		// if the current element's current offset doesn't match the calculated offset, some data has resized and we need to fix...
		OffsetDelta = CurrentOffset - Elements[iElement].Offset;

		switch (iElement)
		{
			case EElement_Name:
			case EElement_GlobalSequence:
			case EElement_Animation:
			case EElement_AnimationLookup:
				break;

			case EElement_Bone:
			{
				CElement_Bone* Bones = Elements[iElement].as<CElement_Bone>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Rotation, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Scale, iElement);
				}
				break;
			}
			case EElement_KeyBoneLookup:
			case EElement_Vertex:
				break;

			case EElement_Color:
			{
				CElement_Color* Colors = Elements[iElement].as<CElement_Color>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Opacity, iElement);
				}
				break;
			}

			case EElement_Texture:
			{
				CElement_Texture* Textures = Elements[iElement].as<CElement_Texture>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					if (Textures[j].TexturePath.Offset)
					{
						VERIFY_OFFSET_LOCAL(Textures[j].TexturePath.Offset);
						Textures[j].TexturePath.Offset += OffsetDelta;
					}
				}
				break;
			}

			case EElement_Transparency:
			{
				CElement_Transparency* Transparencies = Elements[iElement].as<CElement_Transparency>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Transparencies[j].AnimationBlock_Transparency, iElement);
				}
				break;
			}
			case EElement_TextureAnimation:
			{
				CElement_UVAnimation* Animations = Elements[iElement].as<CElement_UVAnimation>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Rotation, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Scale, iElement);
				}
				break;
			}

			case EElement_TextureReplace:
			case EElement_TextureFlags:
			case EElement_SkinnedBoneLookup:
			case EElement_TextureLookup:
			case EElement_TextureUnitLookup:
			case EElement_TransparencyLookup:
			case EElement_TextureAnimationLookup:
			case EElement_BoundingTriangle:
			case EElement_BoundingVertex:
			case EElement_BoundingNormal:
				break;

			case EElement_Attachment:
			{
				CElement_Attachment* Attachments = Elements[iElement].as<CElement_Attachment>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Attachments[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_AttachmentLookup:
				break;

			case EElement_Event:
			{
				CElement_Event* Events = Elements[iElement].as<CElement_Event>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
					m_FixAnimationM2Array(OffsetDelta, totalDiff, Events[j].GlobalSequenceID, Events[j].TimeLines, iElement);

				break;
			}

			case EElement_Light:
			{
				CElement_Light* Lights = Elements[iElement].as<CElement_Light>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AmbientColor, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AmbientIntensity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_DiffuseColor, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_DiffuseIntensity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AttenuationStart, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AttenuationEnd, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_Camera:
			{
				CElement_Camera* Cameras = Elements[iElement].as<CElement_Camera>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Target, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Roll, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_FieldOfView, iElement);
				}
				break;
			}

			case EElement_CameraLookup:
				break;

			case EElement_RibbonEmitter:
			{
				// untested!
				CElement_RibbonEmitter* RibbonEmitters = Elements[iElement].as<CElement_RibbonEmitter>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					VERIFY_OFFSET_LOCAL(RibbonEmitters[j].Texture.Offset);
					RibbonEmitters[j].Texture.Offset += OffsetDelta;
					VERIFY_OFFSET_LOCAL(RibbonEmitters[j].RenderFlag.Offset);
					RibbonEmitters[j].RenderFlag.Offset += OffsetDelta;

					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Opacity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_HeightAbove, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_HeightBelow, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_TexSlotTrack, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_ParticleEmitter:
			{
				CElement_ParticleEmitter* ParticleEmitters = Elements[iElement].as<CElement_ParticleEmitter>();
				for (uint32_t j = 0; j < Elements[iElement].Count; ++j)
				{
					if (ParticleEmitters[j].FileNameModel.Count)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].FileNameModel.Offset);
						ParticleEmitters[j].FileNameModel.Offset += OffsetDelta;
					}
					else
						ParticleEmitters[j].FileNameModel.Offset = 0;

					if (ParticleEmitters[j].ChildEmitter.Count)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].ChildEmitter.Offset);
						ParticleEmitters[j].ChildEmitter.Offset += OffsetDelta;
					}
					else
						ParticleEmitters[j].ChildEmitter.Offset = 0;

					if (ParticleEmitters[j].SplinePoints.Count)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].SplinePoints.Offset);
						ParticleEmitters[j].SplinePoints.Offset += OffsetDelta;
					}
					else
						ParticleEmitters[j].SplinePoints.Offset = 0;

					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitSpeed, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_SpeedVariance, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_VerticalRange, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_HorizontalRange, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Gravity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Lifespan, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmissionRate, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmissionAreaLength, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmissionAreaWidth, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_zSource, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EnabledIn, iElement);

					m_FixFakeAnimationBlockOffsets_Old(OffsetDelta, totalDiff, ParticleEmitters[j].ColorTrack, iElement);
					m_FixFakeAnimationBlockOffsets_Old(OffsetDelta, totalDiff, ParticleEmitters[j].AlphaTrack, iElement);
					m_FixFakeAnimationBlockOffsets_Old(OffsetDelta, totalDiff, ParticleEmitters[j].ScaleTrack, iElement);
					m_FixFakeAnimationBlockOffsets_Old(OffsetDelta, totalDiff, ParticleEmitters[j].HeadCellTrack, iElement);
					m_FixFakeAnimationBlockOffsets_Old(OffsetDelta, totalDiff, ParticleEmitters[j].TailCellTrack, iElement);
				}
				break;
			}
		}

		// set the element's new offset
		Elements[iElement].Offset = CurrentOffset;
		if (Elements[iElement].SizeOriginal != Elements[iElement].Data.size())
			sLogger.LogInfo(L"Element #%u size changed", iElement);
		Elements[iElement].SizeOriginal = Elements[iElement].Data.size();
		Elements[iElement].OffsetOriginal = CurrentOffset;
		CurrentOffset += Elements[iElement].Data.size();
	}

	m_OriginalModelChunkSize = GetHeaderSize();
	for (uint32_t iElement = 0; iElement < EElement__Count__; ++iElement)
		m_OriginalModelChunkSize += Elements[iElement].Data.size();
}

void M2Lib::M2::m_FixAnimationM2Array_Old(int32_t OffsetDelta, int32_t TotalDelta, int16_t GlobalSequenceID, M2Array& Array, int32_t iElement)
{
#define IS_LOCAL_ANIMATION(Offset) \
	(Offset >= Elements[iElement].OffsetOriginal && (Offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size()))

	auto animationElement = GetAnimations();
	m2lib_assert("Failed to get model animations" && animationElement);
	m2lib_assert("Zero animations count for model" && animationElement->Count);

	auto animations = animationElement->as<CElement_Animation>();

	VERIFY_OFFSET_LOCAL(Array.Offset);

	if (GlobalSequenceID == -1)
	{
		if (Array.Count)
		{
			auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);

			for (int32_t i = 0; i < Array.Count; ++i)
			{
				m2lib_assert("Out of animation index" && i < animationElement->Count);
				if (!animations[i].IsInline())
					continue;

				//SubArrays[i].Shift(IS_LOCAL_ANIMATION(SubArrays[i].Offset) ? OffsetDelta : TotalDelta);
				if (SubArrays[i].Offset >= Elements[iElement].OffsetOriginal && (SubArrays[i].Offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size()))
					SubArrays[i].Shift(OffsetDelta);
				else
					SubArrays[i].Shift(TotalDelta);
			}
		}

		Array.Shift(IS_LOCAL_ANIMATION(Array.Offset) ? OffsetDelta : TotalDelta);
	}
	else
	{
		auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);
		for (int32_t i = 0; i < Array.Count; ++i)
			SubArrays[i].Shift(IS_LOCAL_ANIMATION(SubArrays[i].Offset) ? OffsetDelta : TotalDelta);

		Array.Shift(IS_LOCAL_ANIMATION(Array.Offset) ? OffsetDelta : TotalDelta);
	}
}

void M2Lib::M2::m_FixAnimationM2Array(int32_t OffsetDelta, int32_t TotalDelta, int16_t GlobalSequenceID, M2Array& Array, int32_t iElement)
{
	if (!Array.Count)
		return;

	auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);
	auto& Element = Elements[iElement];

	for (int32_t i = 0; i < Array.Count; ++i)
	{
		if (!SubArrays[i].Offset)
			continue;

		m2lib_assert("i < animation.count" && i < GetAnimations()->Count);
		auto animation = GetAnimations()->at<CElement_Animation>(i);

		/*auto lte = SubArrays[i].Offset < Element.Offset ? 1 : 0;
		auto ext = SubArrays[i].Offset > (uint32_t)Element.Data.size() + Element.Offset ? 1 : 0;
		auto flags = (animation->Flags & 0x20) != 0 ? 1 : 0;
		if ((lte == 0 && ext == 1 && flags == 0 || i == 68) && iElement == EElement_Transparency)
		{
			int a = 1;
		}
		sLogger.LogInfo("lte:%u ext:%u seq:%i flags:%X flags&20:%u",
			lte,
			ext,
			GlobalSequenceID,
			animation->Flags,
			flags);*/

		if (animation->IsInline())
		{
			// we dont know actual particle emitter block size...
			m2lib_assert("Not external offset" && (iElement == GetLastElementIndex() || SubArrays[i].Offset > Elements[iElement].Offset + Elements[iElement].SizeOriginal));
			SubArrays[i].Shift(TotalDelta);
		}
	}

	Array.Shift(OffsetDelta);
}

void M2Lib::M2::m_FixAnimationOffsets_Old(int32_t OffsetDelta, int32_t TotalDelta, CElement_AnimationBlock& AnimationBlock, int32_t iElement)
{
	m_FixAnimationM2Array_Old(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array_Old(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixAnimationOffsets(int32_t OffsetDelta, int32_t TotalDelta, CElement_AnimationBlock& AnimationBlock, int32_t iElement)
{
	if (Settings.FixAnimationsTest)
	{
		if (AnimationBlock.Times.Count != AnimationBlock.Keys.Count)
			return;
	}

	//auto animationElement = GetAnimations();
	//m2lib_assert("count(anims) != M2Array.Count" && AnimationBlock.Times.Count == animationElement->Count);
	//sLogger.LogInfo("seq:%i count:%u anims:%u eq:%u", AnimationBlock.GlobalSequenceID, AnimationBlock.Times.Count,animationElement->Count, AnimationBlock.Times.Count == animationElement->Count ? 1 :0);

	m_FixAnimationM2Array(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixFakeAnimationBlockOffsets(int32_t OffsetDelta, int32_t TotalDelta, CElement_FakeAnimationBlock& AnimationBlock, int32_t iElement)
{
	if (Settings.FixAnimationsTest)
	{
		if (AnimationBlock.Times.Count != AnimationBlock.Keys.Count)
			return;
	}

	//auto animationElement = GetAnimations();
	//m2lib_assert("count(anims) != M2Array.Count" && AnimationBlock.Times.Count == animationElement->Count);
	//sLogger.LogInfo("seq:%i count:%u anims:%u eq:%u", AnimationBlock.GlobalSequenceID, AnimationBlock.Times.Count,animationElement->Count, AnimationBlock.Times.Count == animationElement->Count ? 1 :0);

	m_FixAnimationM2Array(OffsetDelta, TotalDelta, -1, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array(OffsetDelta, TotalDelta, -1, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixFakeAnimationBlockOffsets_Old(int32_t OffsetDelta, int32_t TotalDelta, CElement_FakeAnimationBlock& AnimationBlock, int32_t iElement)
{
	// TP is the best
	if (AnimationBlock.Times.Count)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.Times.Offset);

		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.Times.Offset) && (AnimationBlock.Times.Offset < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		m2lib_assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.Times.Offset);
		m2lib_assert(AnimationBlock.Times.Offset > 0);
		AnimationBlock.Times.Offset += OffsetDelta;
	}

	if (AnimationBlock.Keys.Count)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.Keys.Offset);
		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.Keys.Offset) && (AnimationBlock.Keys.Offset < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		m2lib_assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.Keys.Offset);
		m2lib_assert(AnimationBlock.Keys.Offset > 0);
		AnimationBlock.Keys.Offset += OffsetDelta;
	}
}

void M2Lib::M2::m_SaveElements_CopyElementsToHeader()
{
	Header.Description.nName = Elements[EElement_Name].Count;
	Header.Description.oName = Elements[EElement_Name].Offset;

	Header.Elements.nGlobalSequence = Elements[EElement_GlobalSequence].Count;
	Header.Elements.oGlobalSequence = Elements[EElement_GlobalSequence].Offset;

	Header.Elements.nAnimation = Elements[EElement_Animation].Count;
	Header.Elements.oAnimation = Elements[EElement_Animation].Offset;

	Header.Elements.nAnimationLookup = Elements[EElement_AnimationLookup].Count;
	Header.Elements.oAnimationLookup = Elements[EElement_AnimationLookup].Offset;

	Header.Elements.nBone = Elements[EElement_Bone].Count;
	Header.Elements.oBone = Elements[EElement_Bone].Offset;

	Header.Elements.nKeyBoneLookup = Elements[EElement_KeyBoneLookup].Count;
	Header.Elements.oKeyBoneLookup = Elements[EElement_KeyBoneLookup].Offset;

	Header.Elements.nVertex = Elements[EElement_Vertex].Count;
	Header.Elements.oVertex = Elements[EElement_Vertex].Offset;

	Header.Elements.nColor = Elements[EElement_Color].Count;
	Header.Elements.oColor = Elements[EElement_Color].Offset;

	Header.Elements.nTexture = Elements[EElement_Texture].Count;
	Header.Elements.oTexture = Elements[EElement_Texture].Offset;

	Header.Elements.nTransparency = Elements[EElement_Transparency].Count;
	Header.Elements.oTransparency = Elements[EElement_Transparency].Offset;

	Header.Elements.nTextureAnimation = Elements[EElement_TextureAnimation].Count;
	Header.Elements.oTextureAnimation = Elements[EElement_TextureAnimation].Offset;

	Header.Elements.nTextureReplace = Elements[EElement_TextureReplace].Count;
	Header.Elements.oTextureReplace = Elements[EElement_TextureReplace].Offset;

	Header.Elements.nTextureFlags = Elements[EElement_TextureFlags].Count;
	Header.Elements.oTextureFlags = Elements[EElement_TextureFlags].Offset;

	Header.Elements.nSkinnedBoneLookup = Elements[EElement_SkinnedBoneLookup].Count;
	Header.Elements.oSkinnedBoneLookup = Elements[EElement_SkinnedBoneLookup].Offset;

	Header.Elements.nTextureLookup = Elements[EElement_TextureLookup].Count;
	Header.Elements.oTextureLookup = Elements[EElement_TextureLookup].Offset;

	Header.Elements.nTextureUnitLookup = Elements[EElement_TextureUnitLookup].Count;
	Header.Elements.oTextureUnitLookup = Elements[EElement_TextureUnitLookup].Offset;

	Header.Elements.nTransparencyLookup = Elements[EElement_TransparencyLookup].Count;
	Header.Elements.oTransparencyLookup = Elements[EElement_TransparencyLookup].Offset;

	Header.Elements.nTextureAnimationLookup = Elements[EElement_TextureAnimationLookup].Count;
	Header.Elements.oTextureAnimationLookup = Elements[EElement_TextureAnimationLookup].Offset;

	Header.Elements.nBoundingTriangle = Elements[EElement_BoundingTriangle].Count;
	Header.Elements.oBoundingTriangle = Elements[EElement_BoundingTriangle].Offset;

	Header.Elements.nBoundingVertex = Elements[EElement_BoundingVertex].Count;
	Header.Elements.oBoundingVertex = Elements[EElement_BoundingVertex].Offset;

	Header.Elements.nBoundingNormal = Elements[EElement_BoundingNormal].Count;
	Header.Elements.oBoundingNormal = Elements[EElement_BoundingNormal].Offset;

	Header.Elements.nAttachment = Elements[EElement_Attachment].Count;
	Header.Elements.oAttachment = Elements[EElement_Attachment].Offset;

	Header.Elements.nAttachmentLookup = Elements[EElement_AttachmentLookup].Count;
	Header.Elements.oAttachmentLookup = Elements[EElement_AttachmentLookup].Offset;

	Header.Elements.nEvent = Elements[EElement_Event].Count;
	Header.Elements.oEvent = Elements[EElement_Event].Offset;

	Header.Elements.nLight = Elements[EElement_Light].Count;
	Header.Elements.oLight = Elements[EElement_Light].Offset;

	Header.Elements.nCamera = Elements[EElement_Camera].Count;
	Header.Elements.oCamera = Elements[EElement_Camera].Offset;

	Header.Elements.nCameraLookup = Elements[EElement_CameraLookup].Count;
	Header.Elements.oCameraLookup = Elements[EElement_CameraLookup].Offset;

	Header.Elements.nRibbonEmitter = Elements[EElement_RibbonEmitter].Count;
	Header.Elements.oRibbonEmitter = Elements[EElement_RibbonEmitter].Offset;

	Header.Elements.nParticleEmitter = Elements[EElement_ParticleEmitter].Count;
	Header.Elements.oParticleEmitter = Elements[EElement_ParticleEmitter].Offset;

	if (Header.IsLongHeader() && GetExpansion() >= Expansion::Cataclysm)
	{
		Header.Elements.nTextureCombinerCombo = Elements[EElement_TextureCombinerCombo].Count;
		Header.Elements.oTextureCombinerCombo = Elements[EElement_TextureCombinerCombo].Offset;
	}
}

uint32_t M2Lib::M2::AddTexture(CElement_Texture::ETextureType Type, CElement_Texture::ETextureFlags Flags, char const* szTextureSource)
{
	if (Type != CElement_Texture::ETextureType::Final_Hardcoded)
		szTextureSource = "";

	auto& Element = Elements[EElement_Texture];

	// shift offsets for existing textures
	for (uint32_t i = 0; i < Element.Count; ++i)
	{
		auto texture = Element.at<CElement_Texture>(i);
		if (texture->TexturePath.Offset)
			texture->TexturePath.Offset += sizeof(CElement_Texture);
	}

	// add element placeholder for new texture
	Element.Data.insert(Element.Data.begin() + Element.Count * sizeof(CElement_Texture), sizeof(CElement_Texture), 0);

	bool inplacePath = true;
	auto textureChunk = (TXIDChunk*)GetChunk(EM2Chunk::Texture);

	if (strlen(szTextureSource) > 0)
		sLogger.LogInfo(L"Adding custom texture %s", StringHelpers::StringToWString(szTextureSource).c_str());

	if (textureChunk)
	{
		if (!strlen(szTextureSource))
		{
			inplacePath = false;
			textureChunk->TextureFileDataIds.push_back(0);
		}
		else if (auto info = storageRef->GetFileInfoByPath(StringHelpers::StringToWString(szTextureSource)))
		{
			sLogger.LogInfo(L"Texture '%s' is indexed in CASC by FileDataId = %u", StringHelpers::StringToWString(szTextureSource).c_str(), info->FileDataId);
			inplacePath = false;
			textureChunk->TextureFileDataIds.push_back(info->FileDataId);
		}
		else {
			textureChunk->TextureFileDataIds.push_back(0);

			sLogger.LogInfo(L"Texture '%s' is not indexed in CASC, TXID chunk will be removed from model", StringHelpers::StringToWString(szTextureSource).c_str());
			// texture is not indexed, texture chunk needs to be removed
			needRemoveTXIDChunk = true;
		}
	}

	auto newIndex = Element.Count;

	if (inplacePath)
	{
		auto texturePathPos = Element.Data.size();
		// add placeholder for texture path
		Element.Data.insert(Element.Data.end(), strlen(szTextureSource) + 1, 0);

		CElement_Texture& newTexture = Element.as<CElement_Texture>()[newIndex];
		newTexture.Type = Type;
		newTexture.Flags = Flags;
		newTexture.TexturePath.Count = strlen(szTextureSource) + 1;
		newTexture.TexturePath.Offset = Element.Offset + texturePathPos;

		memcpy(&Element.Data[texturePathPos], szTextureSource, newTexture.TexturePath.Count);
	}
	else
	{
		CElement_Texture& newTexture = Element.as<CElement_Texture>()[newIndex];
		newTexture.Type = Type;
		newTexture.Flags = Flags;
	}

	++Element.Count;

	return newIndex;
}

uint32_t M2Lib::M2::CloneTexture(uint16_t TextureId)
{
	m2lib_assert(TextureId < Header.Elements.nTexture && "Too large texture index");

	auto& texture = Elements[EElement_Texture].as<CElement_Texture>()[TextureId];
	std::string texturePath = (char*)Elements[EElement_Texture].GetLocalPointer(texture.TexturePath.Offset);

	return AddTexture(texture.Type, texture.Flags, texturePath.c_str());
}

uint32_t M2Lib::M2::AddTextureFlags(CElement_TextureFlag::EFlags Flags, CElement_TextureFlag::EBlend Blend)
{
	auto& Element = Elements[EElement_TextureFlags];
	auto newIndex = Element.Count;

	Element.Data.insert(Element.Data.end(), sizeof(CElement_TextureFlag), 0);
	CElement_TextureFlag& newFlags = Element.as<CElement_TextureFlag>()[newIndex];
	newFlags.Flags = Flags;
	newFlags.Blend = Blend;

	if (auto TXACChunk = (M2Chunk::TXACChunk*)GetChunk(EM2Chunk::TXAC))
	{
		M2Chunk::TXACChunk::texture_ac newAc;
		newAc.unk[0] = 0;
		newAc.unk[1] = 0;
		TXACChunk->TextureFlagsAC.push_back(newAc);
	}

	++Element.Count;
	return newIndex;
}

uint32_t M2Lib::M2::GetTextureIndex(M2Element::CElement_Texture::ETextureType Type, const char* szTextureSource)
{
	if (Type == CElement_Texture::ETextureType::Final_Hardcoded)
	{
		if (auto textureChunk = (M2Chunk::TXIDChunk*)GetChunk(EM2Chunk::Texture))
		{
			// if file not found - can be custom texture
			if (auto info = storageRef->GetFileInfoByPath(StringHelpers::StringToWString(szTextureSource).c_str()))
			{
				for (uint32_t i = 0; i < textureChunk->TextureFileDataIds.size(); ++i)
					if (textureChunk->TextureFileDataIds[i] == info->FileDataId)
						return i;
			}
		}
	}

	auto& Element = Elements[EElement_Texture];
	for (uint32_t i = 0; i < Element.Count; ++i)
	{
		auto& texture = Element.as<CElement_Texture>()[i];
		if (texture.Type != Type)
			continue;

		if (Type != CElement_Texture::ETextureType::Final_Hardcoded)
			return i;

		if (texture.TexturePath.Offset)
		{
			auto pTexturePath = (char const*)Element.GetLocalPointer(texture.TexturePath.Offset);
			if (CalcStringHash<char>(pTexturePath) == CalcStringHash<char>(szTextureSource))
				return i;
		}
	}

	return -1;
}

std::wstring M2Lib::M2::GetTexturePath(uint32_t Index)
{
	auto& TextureElement = Elements[EElement_Texture];
	m2lib_assert(__FUNCTION__ "Texture index too large" && Index < TextureElement.Count);

	auto texture = TextureElement.at<CElement_Texture>(Index);

	if (texture->Type != M2Element::CElement_Texture::ETextureType::Final_Hardcoded)
		return L"";

	if (texture->TexturePath.Offset && texture->TexturePath.Count)
		return StringHelpers::StringToWString((char*)TextureElement.GetLocalPointer(texture->TexturePath.Offset)).c_str();

	auto textureChunk = (TXIDChunk*)GetChunk(EM2Chunk::Texture);
	if (!textureChunk || textureChunk->TextureFileDataIds.size() <= Index)
		return L"";

	return storageRef->GetFileInfoByFileDataId(textureChunk->TextureFileDataIds[Index])->Path;
}

void M2Lib::M2::RemoveTXIDChunk()
{
	sLogger.LogInfo(L"Erasing TXID chunk from model");

	auto chunkItr = Chunks.find(EM2Chunk::Texture);
	if (chunkItr == Chunks.end())
	{
		sLogger.LogInfo(L"TXID chunk not present, skipping");
		return;
	}

	auto chunk = (M2Chunk::TXIDChunk*)chunkItr->second;

	uint32_t newDataLen = 0;
	std::map<uint32_t, std::string> PathsByTextureId;

	auto& Element = Elements[EElement_Texture];
	for (uint32_t i = 0; i < chunk->TextureFileDataIds.size(); ++i)
	{
		m2lib_assert(i < Element.Count);
		auto FileDataId = chunk->TextureFileDataIds[i];
		if (!FileDataId)
			continue;

		auto info = storageRef->GetFileInfoByFileDataId(FileDataId);
		if (!info)
		{
			sLogger.LogError(L"Error: failed to get path for FileDataId = [%u] for texture #%u. FileStorage is not initialized or listfile is not loaded or not up to date", FileDataId, i);
			sLogger.LogError(L"Custom textures will not work ingame");
			return;
		}

		PathsByTextureId[i] = StringHelpers::WStringToString(info->Path);
		newDataLen += info->Path.length() + 1;
	}

	sLogger.LogInfo(L"Total bytes for storing textures will be used: %u", newDataLen);

	if (!newDataLen)
	{
		Chunks.erase(chunkItr);
		return;
	}

	uint32_t pathOffset = 0;
	uint32_t OldSize = Element.Data.size();
	Element.Data.insert(Element.Data.end(), newDataLen, 0);
	for (auto itr : PathsByTextureId)
	{
		auto texture = Element.at<CElement_Texture>(itr.first);
		texture->TexturePath.Offset = Element.Offset + OldSize + pathOffset;
		texture->TexturePath.Count = itr.second.length() + 1;

		memcpy(&Element.Data[OldSize + pathOffset], itr.second.data(), itr.second.length());
		pathOffset += itr.second.length() + 1;
	}

	sLogger.LogInfo(L"Moved %u textures from chunk", PathsByTextureId.size());

	Chunks.erase(chunkItr);
}

uint32_t M2Lib::M2::AddTextureLookup(uint16_t TextureId, bool ForceNewIndex /*= false*/)
{
	auto& Element = Elements[EElement_TextureLookup];

	if (!ForceNewIndex)
	{
		for (uint32_t i = 0; i < Element.Count; ++i)
		{
			auto& lookup = Element.as<CElement_TextureLookup>()[i];
			if (lookup.TextureIndex == TextureId)
				return i;
		}
	}

	// add element placeholder for new lookup
	Element.Data.insert(Element.Data.begin() + Element.Count * sizeof(CElement_TextureLookup), sizeof(CElement_TextureLookup), 0);

	auto newIndex = Element.Count;
	CElement_TextureLookup& newLookup = Element.as<CElement_TextureLookup>()[newIndex];
	newLookup.TextureIndex = TextureId;

	++Element.Count;
	return newIndex;
}

uint32_t M2Lib::M2::AddBone(CElement_Bone const& Bone)
{
	auto& BoneElement = Elements[M2Element::EElement_Bone];
	auto newBoneId = BoneElement.Count;

	auto Bones = Elements[M2Element::EElement_Bone].as<CElement_Bone>();
	for (uint32_t i = 0; i < BoneElement.Count; ++i)
	{
		m_FixAnimationOffsets(sizeof(CElement_Bone), 0, Bones[i].AnimationBlock_Position, EElement_Bone);
		m_FixAnimationOffsets(sizeof(CElement_Bone), 0, Bones[i].AnimationBlock_Rotation, EElement_Bone);
		m_FixAnimationOffsets(sizeof(CElement_Bone), 0, Bones[i].AnimationBlock_Scale, EElement_Bone);
	}

	BoneElement.Data.insert(BoneElement.Data.begin() + BoneElement.Count * sizeof(CElement_Bone), sizeof(CElement_Bone), 0);
	Elements[M2Element::EElement_Bone].as<CElement_Bone>()[newBoneId] = Bone;

	++BoneElement.Count;

	return newBoneId;
}

M2LIB_HANDLE M2Lib::M2_Create(Settings* settings)
{
	try
	{
		return static_cast<M2LIB_HANDLE>(new M2(settings));
	}
	catch (std::exception& e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return nullptr;
	}
}

M2Lib::EError M2Lib::M2_Load(M2LIB_HANDLE pointer, const wchar_t* FileName)
{
	try
	{
		return static_cast<M2*>(pointer)->Load(FileName);
	}
	catch (std::exception& e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return EError_FAIL;
	}
}

M2Lib::EError M2Lib::M2_Save(M2LIB_HANDLE handle, const wchar_t* FileName)
{
	try
	{
		return static_cast<M2*>(handle)->Save(FileName);
	}
	catch (std::exception& e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return EError_FAIL;
	}
}

M2Lib::EError M2Lib::M2_SetReplaceM2(M2LIB_HANDLE handle, const wchar_t* FileName)
{
	try
	{
		return static_cast<M2*>(handle)->SetReplaceM2(FileName);
	}
	catch (std::exception& e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return EError_FAIL;
	}
}

M2Lib::EError M2Lib::M2_ExportM2Intermediate(M2LIB_HANDLE handle, const wchar_t* FileName)
{
	try
	{
		return static_cast<M2*>(handle)->ExportM2Intermediate(FileName);
	}
	catch (std::exception& e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return EError_FAIL;
	}
}

M2Lib::EError M2Lib::M2_ImportM2Intermediate(M2LIB_HANDLE handle, const wchar_t* FileName)
{
	try
	{
		return static_cast<M2*>(handle)->ImportM2Intermediate(FileName);
	}
	catch (std::exception& e)
	{
		sLogger.LogError(L"Exception: %s", StringHelpers::StringToWString(e.what()).c_str());

		return EError_FAIL;
	}
}

void M2Lib::M2_Free(M2LIB_HANDLE handle)
{
	delete static_cast<M2*>(handle);
}
