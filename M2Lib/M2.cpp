#include "M2.h"
#include "DataBinary.h"
#include "M2SkinBuilder.h"
#include "Settings.h"
#include "Skeleton.h"
#include "FileSystem.h"
#include "Casc.h"
#include "Logger.h"
#include <sstream>
#include <locale>
#include <string>
#include <codecvt>

using namespace M2Lib::M2Element;
using namespace M2Lib::M2Chunk;

// level of detail for output messages
int g_Verbose = 1;

M2Lib::DataElement* M2Lib::M2::GetLastElement()
{
	for (int i = M2Element::EElement__Count__ - 1; i >= 0; --i)
	{
		if (!Elements[i].Data.empty())
			return &Elements[i];
	}

	return NULL;
}

M2Lib::Expansion M2Lib::M2::GetExpansion() const
{
	if (Settings && Settings->ForceLoadExpansion  != Expansion::None)
		return Settings->ForceLoadExpansion;

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

M2Lib::M2::~M2()
{
	delete pInM2I;
	for (auto& Chunk : Chunks)
		delete Chunk.second;
	Chunks.clear();
	delete Skeleton;
	delete ParentSkeleton;
	delete replaceM2;

	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
		delete Skins[i];

	if (auto casc = GetCasc())
		casc->RemoveReference();
}

UInt32 M2Lib::M2::GetHeaderSize() const
{
	return Header.IsLongHeader() && GetExpansion() >= Expansion::Cataclysm ? sizeof(Header) : sizeof(Header) - 8;
}

std::wstring M2Lib::M2::GetModelDirectory() const
{
	return FileSystemW::GetParentDirectory(_FileName);
}

M2Lib::EError M2Lib::M2::Load(const Char16* FileName)
{
	sLogger.Log("Loading file %s", std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(FileName).c_str());

	// check path
	if (!FileName)
	{
		sLogger.Log("Error: No file specified");
		return EError_FailedToLoadM2_NoFileSpecified;
	}

	_FileName = FileName;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
	{
		sLogger.Log("Error: Failed to open file");
		return EError_FailedToLoadM2_CouldNotOpenFile;
	}

	// find file size
	FileStream.seekg(0, std::ios::end);
	UInt32 FileSize = (UInt32)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	sLogger.Log("File size: %u", FileSize);

	struct PostChunkInfo
	{
		PostChunkInfo() { }
		PostChunkInfo(UInt32 Offs, UInt32 Size)
		{
			this->Offs = Offs;
			this->Size = Size;
		}

		UInt32 Offs;
		UInt32 Size;
	};

	std::map<EM2Chunk, PostChunkInfo> PostProcessChunks;

	while (FileStream.tellg() < FileSize)
	{
		UInt32 ChunkId;
		UInt32 ChunkSize;

		FileStream.read((char*)&ChunkId, sizeof(ChunkId));
		FileStream.read((char*)&ChunkSize, sizeof(ChunkSize));

		// support pre-legion M2
		if (REVERSE_CC(ChunkId) == 'MD20')
		{
			sLogger.Log("Detected pre-Legion mode (unchunked)");
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

			sLogger.Log("Loaded '%s' M2 chunk, size %u", ChunkIdToStr(ChunkId, false).c_str(), ChunkSize);
			switch (eChunk)
			{
				case EM2Chunk::Model: Chunk = new MD21Chunk(); break;
				case EM2Chunk::Physic: Chunk = new PFIDChunk(); break;
				case EM2Chunk::Animation: Chunk = new AFIDChunk(); break;
				case EM2Chunk::Bone: Chunk = new BFIDChunk(); break;
				case EM2Chunk::Skeleton: Chunk = new SKIDChunk(); break;
				case EM2Chunk::Skin: Chunk = new SFIDChunk(); break;
				case EM2Chunk::Texture: Chunk = new TXIDChunk(); break;
				default:
					Chunk = new RawChunk();
					break;
			}

			UInt32 savePos = FileStream.tellg();
			Chunk->Load(FileStream, ChunkSize);
			FileStream.seekg(savePos + ChunkSize, std::ios::beg);

			Chunks[eChunk] = Chunk;
		}
	}

	auto ModelChunk = (MD21Chunk*)GetChunk(EM2Chunk::Model);
	if (!ModelChunk)
	{
		sLogger.Log("Error: '%s' chunk not found in model", ChunkIdToStr((UInt32)EM2Chunk::Model, true).c_str());
		return EError_FailedToLoadM2_FileCorrupt;
	}

	m_OriginalModelChunkSize = ModelChunk->RawData.size();

	// load header
	memcpy(&Header, ModelChunk->RawData.data(), sizeof(Header));
	if (!Header.IsLongHeader() || GetExpansion() < Expansion::Cataclysm)
	{
		sLogger.Log("Short header detected");
		Header.Elements.nTextureCombinerCombo = 0;
		Header.Elements.oTextureCombinerCombo = 0;
	}

	if (Header.Description.Version < 263 || Header.Description.Version > 274)
	{
		sLogger.Log("Error: Unsupported model version %u", Header.Description.Version);
		return EError_FailedToLoadM2_VersionNotSupported;
	}

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(m_OriginalModelChunkSize);

	// load elements
	for (UInt32 i = 0; i < EElement__Count__; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load(ModelChunk->RawData.data(), 0))
		{
			sLogger.Log("Error: Failed to load M2 element #%u", i);
			return EError_FailedToLoadM2_FileCorrupt;
		}
	}

	// load skins
	if ((Header.Elements.nSkin == 0) || (Header.Elements.nSkin > SKIN_COUNT - LOD_SKIN_MAX_COUNT))
	{
		sLogger.Log("Error: Unsupported number of skins in model: %u", Header.Elements.nSkin);
		return EError_FailedToLoadM2_FileCorrupt;
	}

	if (auto casc = GetCasc())
	{
		if (auto chunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin))
		{
			sLogger.Log("Used skin files:");
			for (auto fileDataId : chunk->SkinsFileDataIds)
				sLogger.Log("\t[%u] %s", fileDataId, casc->GetFileByFileDataId(fileDataId).c_str());
		}
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
			if (!GetFileSkin(FileNameSkin, _FileName, SKIN_COUNT - LOD_SKIN_MAX_COUNT + i))
				continue;

			sLogger.Log("Loading skin '%s'...", std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(FileSystemW::GetBaseName(FileNameSkin)).c_str());
			M2Skin LoDSkin(this);
			if (EError Error = LoDSkin.Load(FileNameSkin.c_str()))
			{
				if (Error = EError_FailedToLoadSKIN_CouldNotOpenFile)
					continue;

				sLogger.Log("Error: Failed to load #%u lod skin %s", i, std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(FileSystemW::GetBaseName(FileNameSkin)).c_str());
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
			default:
			{
				Chunk = new RawChunk();
				Chunk->Load(FileStream, PostChunk.second.Size);
				break;
			}
		}

		Chunks[PostChunk.first] = Chunk;
	}

	if (auto casc = GetCasc())
	{
		if (auto chunk = (SKIDChunk*)GetChunk(EM2Chunk::Skeleton))
		{
			sLogger.Log("Used skeleton file:");
			sLogger.Log("\t[%u] %s", chunk->SkeletonFileDataId, casc->GetFileByFileDataId(chunk->SkeletonFileDataId).c_str());
		}
	}

	Error = LoadSkeleton();
	if (Error != EError::EError_OK)
		return Error;

	// print info
	PrintInfo();
	PrintReferencedFileInfo();

	sLogger.Log("Finished loading M2");

	//m_SaveElements_FindOffsets();

	// done
	return EError_OK;
}

M2Lib::EError M2Lib::M2::LoadSkeleton()
{
	std::wstring FileNameSkeleton;
	if (GetFileSkeleton(FileNameSkeleton, _FileName))
	{
		auto sk = new M2Lib::Skeleton();
		if (sk->Load(FileNameSkeleton.c_str()) == EError::EError_OK)
		{
			sLogger.Log("Sekeleton file detected and loaded");
			Skeleton = sk;

			if (auto chunk = (SkeletonChunk::SKPDChunk*)Skeleton->GetChunk(SkeletonChunk::ESkeletonChunk::SKPD))
			{
				if (auto casc = GetCasc())
				{
					auto path = casc->GetFileByFileDataId(chunk->Data.ParentSkeletonFileId);
					if (!path.empty())
					{
						auto ParentSkeletonFileName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(FileSystemA::GetBaseName(path));
						ParentSkeletonFileName = FileSystemW::GetParentDirectory(_FileName) + L"\\" + ParentSkeletonFileName;
						auto parentSkeleton = new M2Lib::Skeleton();
						if (parentSkeleton->Load(ParentSkeletonFileName.c_str()))
						{
							sLogger.Log("Parent skeleton file [%u] %s loaded", chunk->Data.ParentSkeletonFileId, std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(ParentSkeletonFileName).c_str());
							ParentSkeleton = parentSkeleton;
						}
						else
						{
							sLogger.Log("Error: Failed to load parent skeleton file [%u] %s", chunk->Data.ParentSkeletonFileId, ParentSkeletonFileName.c_str());
							delete parentSkeleton;
						}
					}
					else
						sLogger.Log("Error: skeleotn has parent skeleton chunk, but parent file not loaded!");
				}
				else
					sLogger.Log("Error: failed to load parent skeleton file [%u], CASC not initialized!", chunk->Data.ParentSkeletonFileId);
			}
		}
		else
		{
			if (GetChunk(EM2Chunk::Skeleton))
				sLogger.Log("Error: model has skeleton chunk, but chunk file not loaded!");
			delete sk;
		}
	}

	return EError_OK;
}

M2Lib::EError M2Lib::M2::LoadSkins()
{
	for (UInt32 i = 0; i < Header.Elements.nSkin; ++i)
	{
		std::wstring FileNameSkin;
		if (!GetFileSkin(FileNameSkin, _FileName, i))
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
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		std::wstring FileNameSkin;
		if (GetFileSkin(FileNameSkin, M2FileName, i))
			_wremove(FileNameSkin.c_str());
	}

	for (UInt32 i = 0; i < Header.Elements.nSkin; ++i)
	{
		std::wstring FileNameSkin;
		if (!GetFileSkin(FileNameSkin, M2FileName, i))
			continue;

		if (EError Error = Skins[i]->Save(FileNameSkin.c_str()))
			return Error;
	}

	auto skinChunk = (M2Chunk::SFIDChunk*)GetChunk(EM2Chunk::Skin);

	// 0x80 = flag_has_lod_skin_files - wrong
	//if (Header.Description.Flags & 0x80)
	UInt32 LodSkinCount = skinChunk ? skinChunk->SkinsFileDataIds.size() - Header.Elements.nSkin : (hasLodSkins ? LOD_SKIN_MAX_COUNT : 0);
	for (UInt32 i = 0; i < LodSkinCount; ++i)
	{
		std::wstring FileNameSkin;
		if (!GetFileSkin(FileNameSkin, M2FileName, i + 4))
			continue;

		if (EError Error = (Skins[1] ? Skins[1] : Skins[0])->Save(FileNameSkin.c_str()))
			return Error;
	}

	return EError::EError_OK;
}

void M2Lib::M2::DoExtraWork()
{
	//auto RenderFlags = Elements[EElement_TextureFlags].as<CElement_TextureFlag>();
	//sLogger.Log("Existing render flags:");
	//for (UInt32 i = 0; i < Elements[EElement_TextureFlags].Count; ++i)
	//	sLogger.Log("\tFlags: %u Blend: %u", RenderFlags[i].Flags, RenderFlags[i].Blend);

	M2SkinElement::TextureLookupRemap textureRemap;

	std::map<int, int> renderFlagsByStyle;
	// first assign textures and gloss
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		auto Skin = Skins[i];
		if (!Skin)
			continue;

		for (UInt32 MeshIndex = 0; MeshIndex < Skin->ExtraDataBySubmeshIndex.size(); ++MeshIndex)
		{
			auto ExtraData = Skin->ExtraDataBySubmeshIndex[MeshIndex];

			// copy materials will be done later
			if (ExtraData->MaterialOverride >= 0)
				continue;

			// assign custom texture
			if (ExtraData->CustomTextureName.length() > 0)
			{
				auto textureId = GetTextureIndex(ExtraData->CustomTextureName.c_str());
				if (textureId == -1)
					textureId = AddTexture(ExtraData->CustomTextureName.c_str(),
						CElement_Texture::ETextureType::Final_Hardcoded,
						CElement_Texture::ETextureFlags::None);

				auto textureLookup = AddTextureLookup(textureId, false);
				if (ExtraData->GlossTextureName.empty() && renderFlagsByStyle.find(ExtraData->TextureStyle) == renderFlagsByStyle.end())
				{
					// 8.x: breaks model
					//renderFlagsByStyle[ExtraData->TextureStyle] = AddTextureFlags(CElement_TextureFlag::EFlags_TwoSided,
					//	(CElement_TextureFlag::EBlend)ExtraData->TextureStyle);
					//	CElement_TextureFlag::EBlend::EBlend_Opaque);
				}

				auto Materials = Skin->Elements[M2SkinElement::EElement_Material].as<M2SkinElement::CElement_Material>();
				for (UInt32 j = 0; j < Skin->Header.nMaterial; ++j)
				{
					if (Materials[j].iSubMesh != MeshIndex)
						continue;

					auto& Material = Materials[j];

					Material.textureComboIndex = textureLookup;
					Material.op_count = 1;
					if (ExtraData->GlossTextureName.empty())
					{
						Materials[j].shader_id = TRANSPARENT_SHADER_ID;
						// 8.x: breaks model
						//Materials[j].iRenderFlags = renderFlagsByStyle[ExtraData->TextureStyle];
					}
				}

				auto Batches = Skin->Elements[M2SkinElement::EElement_Flags].as<M2SkinElement::CElement_Flags>();
				for (UInt32 j = 0; j < Skin->Header.nFlags; ++j)
				{
					if (Batches[j].iSubMesh != MeshIndex)
						continue;

					if (Batches[j].TextureId >= 0)
						Batches[j].TextureId = textureId;
				}
			}

			// make glossy
			if (!ExtraData->GlossTextureName.empty())
			{
				std::vector<UInt32> MeshIndexes = { MeshIndex };
				Skin->MakeGlossy(ExtraData->GlossTextureName.c_str(), MeshIndexes, textureRemap);
			}
		}
	}

	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		auto Skin = Skins[i];
		if (!Skin)
			continue;

		// copy materials
		for (UInt32 MeshIndex = 0; MeshIndex < Skin->ExtraDataBySubmeshIndex.size(); ++MeshIndex)
		{
			auto ExtraData = Skin->ExtraDataBySubmeshIndex[MeshIndex];
			if (ExtraData->MaterialOverride < 0)
				continue;

			if (i == 0)
				Skin->CopyMaterial(ExtraData->MaterialOverride, MeshIndex);
			else
			{
				// translate index from zero skin to current skin
				for (UInt32 LocalMeshIndex = 0; LocalMeshIndex < Skin->ExtraDataBySubmeshIndex.size(); ++LocalMeshIndex)
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

void M2Lib::M2::PrepareChunks()
{
	// TODO: leave only non-lod filedataids in skin chunk?

	sLogger.Log("Started preparing chunks for model");
	if (replaceM2)
	{
		auto otherSkinChunk = (SFIDChunk*)replaceM2->GetChunk(EM2Chunk::Skin);
		if (!otherSkinChunk)
		{
			sLogger.Log("Model to replace does not have SKIN chunk, removing source one...");
			RemoveChunk(EM2Chunk::Skin);
		}
		else
		{
			sLogger.Log("Copying skin chunk to source model...");
			Chunks[EM2Chunk::Skin] = otherSkinChunk;
			Header.Elements.nSkin = replaceM2->Header.Elements.nSkin;
		}

		auto otherSkeletonChunk = replaceM2->GetChunk(EM2Chunk::Skeleton);
		if (!otherSkeletonChunk)
		{
			if (GetChunk(EM2Chunk::Skeleton))
				sLogger.Log("Warning: replaced model does not have skeleton file, but source does. Replaced model will use source skeleton file which may cause explosions");
		}
		else
		{
			sLogger.Log("Copying skeleton chunk to source model...");
			Chunks[EM2Chunk::Skeleton] = otherSkinChunk;
		}
	}

	if (auto skinChunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin))
	{
		SInt32 SkinDiff = (replaceM2 ? replaceM2->Header.Elements.nSkin : OriginalSkinCount) - Header.Elements.nSkin;
		if (SkinDiff > 0)
		{
			sLogger.Log("There was %u more skins in original M2, removing extra from skin chunk", SkinDiff);
			for (UInt32 i = 0; i < (UInt32)SkinDiff; ++i)
				skinChunk->SkinsFileDataIds.erase(skinChunk->SkinsFileDataIds.begin() + Header.Elements.nSkin);
		}
		else if (SkinDiff < 0)
		{
			sLogger.Log("Generated model has %u more skins than needed, removing extra", -SkinDiff);
			Header.Elements.nSkin = replaceM2 ? replaceM2->Header.Elements.nSkin : OriginalSkinCount;
		}
	}
}

M2Lib::EError M2Lib::M2::Save(const Char16* FileName, M2* replaceM2)
{
	// check path
	if (!FileName)
		return EError_FailedToSaveM2_NoFileSpecified;

	if (replaceM2)
		this->replaceM2 = replaceM2;

	DoExtraWork();

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToSaveM2;

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// Reserve model chunk header
	UInt32 const ChunkReserveOffset = 8;

	UInt32 ChunkId = REVERSE_CC((UInt32)EM2Chunk::Model);
	FileStream.write((Char8*)&ChunkId, 4);
	FileStream.seekp(4, std::ios::cur);		// reserve bytes for chunk size

	//Header.Description.Version = 0x0110;
	//Header.Description.Flags &= ~0x80;

	// save header
	UInt32 HeaderSize = GetHeaderSize();
	FileStream.write((Char8*)&Header, HeaderSize);

	// save elements
	UInt32 ElementCount = Header.IsLongHeader() ? EElement__Count__ : EElement__Count__ - 1;
	for (UInt32 i = 0; i < ElementCount; ++i)
	{
		if (!Elements[i].Save(FileStream, ChunkReserveOffset))
			return EError_FailedToSaveM2;
	}

	UInt32 MD20Size = (UInt32)FileStream.tellp();
	MD20Size -= ChunkReserveOffset;

	FileStream.seekp(4, std::ios::beg);
	FileStream.write((Char8*)(&MD20Size), 4);

	FileStream.seekp(0, std::ios::end);

	PrepareChunks();

	for (auto chunk : Chunks)
	{
		if (chunk.first == EM2Chunk::Model)
			continue;

		//if (chunk.first == 'SFID')
		//	continue;

		UInt32 ChunkId = REVERSE_CC((UInt32)chunk.first);

		FileStream.write((char*)&ChunkId, 4);
		FileStream.seekp(4, std::ios::cur);		// reserve space for chunk size
		UInt32 savePos = (UInt32)FileStream.tellp();
		chunk.second->Save(FileStream);
		UInt32 ChunkSize = (UInt32)FileStream.tellp() - savePos;
		FileStream.seekp(savePos - 4, std::ios::beg);
		FileStream.write((char*)&ChunkSize, 4);
		FileStream.seekp(0, std::ios::end);
	}

	// save skins
	auto Error = SaveSkins(FileName);
	if (Error != EError_OK)
		return Error;

	if (Skeleton)
	{
		std::wstring SkeletonFileName;
		if (GetFileSkeleton(SkeletonFileName, FileName))
		{
			Error = Skeleton->Save(SkeletonFileName.c_str());
			if (Error != EError_OK)
				return Error;
		}
	}

	if (auto casc = GetCasc())
	{
		if (auto chunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin))
		{
			sLogger.Log("INFO: Put your skins to:");
			for (auto fileDataId : chunk->SkinsFileDataIds)
				sLogger.Log("\t%s", casc->GetFileByFileDataId(fileDataId).c_str());
		}

		if (auto chunk = (SKIDChunk*)GetChunk(EM2Chunk::Skeleton))
		{
			sLogger.Log("INFO: Put your skeleton to:");
			sLogger.Log("\t%s", casc->GetFileByFileDataId(chunk->SkeletonFileDataId).c_str());
		}
	}

	return EError_OK;
}

M2Lib::EError M2Lib::M2::ExportM2Intermediate(Char16 const* FileName)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToExportM2I_CouldNotOpenFile;

	// open binary stream
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// get data to save
	M2Skin* pSkin = Skins[0];

	UInt32 SubsetCount = pSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* Subsets = pSkin->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	CVertex* Vertices = Elements[EElement_Vertex].as<CVertex>();
	UInt16* Triangles = pSkin->Elements[M2SkinElement::EElement_TriangleIndex].as<UInt16>();
	UInt16* Indices = pSkin->Elements[M2SkinElement::EElement_VertexLookup].as<UInt16>();

	auto boneElement = GetBones();
	auto attachmentElement = GetAttachments();

	UInt32 CamerasCount = Elements[EElement_Camera].Count;

	// save signature
	DataBinary.WriteFourCC(M2I::Signature_M2I0);

	// save version
	DataBinary.WriteUInt16(8);
	DataBinary.WriteUInt16(0);

	// save subsets
	DataBinary.WriteUInt32(SubsetCount);
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		M2SkinElement::CElement_SubMesh* pSubsetOut = &Subsets[i];

		DataBinary.WriteUInt16(pSubsetOut->ID);
		DataBinary.WriteASCIIString("");	// description
		DataBinary.WriteSInt16(-1);			// material override
		DataBinary.WriteUInt8(0);			// no custom texture
		DataBinary.WriteASCIIString("");	// custom texture
		DataBinary.WriteUInt16(0);			// texture style
		DataBinary.WriteUInt8(0);			// no gloss texture
		DataBinary.WriteASCIIString("");	// gloss texture
		DataBinary.WriteUInt32(i);			// original subset index

		DataBinary.WriteUInt16(pSubsetOut->Level);

		// write vertices
		DataBinary.WriteUInt32(pSubsetOut->VertexCount);
		UInt32 VertexEnd = pSubsetOut->VertexStart + pSubsetOut->VertexCount;
		for (UInt32 k = pSubsetOut->VertexStart; k < VertexEnd; ++k)
		{
			CVertex const& Vertex = Vertices[Indices[k]];

			DataBinary.WriteC3Vector(Vertex.Position);

			for (UInt32 j = 0; j < BONES_PER_VERTEX; ++j)
				DataBinary.WriteUInt8(Vertex.BoneWeights[j]);

			for (UInt32 j = 0; j < BONES_PER_VERTEX; ++j)
				DataBinary.WriteUInt8(Vertex.BoneIndices[j]);

			DataBinary.WriteC3Vector(Vertex.Normal);

			DataBinary.WriteC2Vector(Vertex.Texture);
			DataBinary.WriteC2Vector(Vertex.Texture2);
		}

		// write triangles
		UInt32 SubsetTriangleCountOut = pSubsetOut->TriangleIndexCount / 3;
		DataBinary.WriteUInt32(SubsetTriangleCountOut);

		// FMN 2015-01-26: changing TriangleIndexstart, depending on ID. See http://forums.darknestfantasyerotica.com/showthread.php?20446-TUTORIAL-Here-is-how-WoD-.skin-works.&p=402561
		UInt32 TriangleIndexStart = 0;

		TriangleIndexStart = UInt32(pSubsetOut->TriangleIndexStart) + (pSubsetOut->Level << 16);

		//UInt32 TriangleIndexEnd = pSubsetOut->TriangleIndexStart + pSubsetOut->TriangleIndexCount;
		//for (UInt32 k = pSubsetOut->TriangleIndexStart; k < TriangleIndexEnd; k++)
		UInt32 TriangleIndexEnd = TriangleIndexStart + pSubsetOut->TriangleIndexCount;
		for (UInt32 k = TriangleIndexStart; k < TriangleIndexEnd; k++)
		{
			UInt16 TriangleIndexOut = Triangles[k] - pSubsetOut->VertexStart;
			assert(TriangleIndexOut < pSubsetOut->VertexCount);
			DataBinary.WriteUInt16(TriangleIndexOut);
		}
	}

	// write bones
	DataBinary.WriteUInt32(boneElement->Count);
	for (UInt16 i = 0; i < boneElement->Count; i++)
	{
		CElement_Bone& Bone = *boneElement->at<CElement_Bone>(i);

		DataBinary.WriteUInt16(i);
		DataBinary.WriteSInt16(Bone.ParentBone);
		DataBinary.WriteC3Vector(Bone.Position);
		DataBinary.WriteUInt8(1);	// has data
		DataBinary.WriteUInt32(Bone.Flags);
		DataBinary.WriteUInt16(Bone.SubmeshId);
		DataBinary.WriteUInt16(Bone.Unknown[0]);
		DataBinary.WriteUInt16(Bone.Unknown[1]);
	}

	// write attachments
	DataBinary.WriteUInt32(attachmentElement->Count);
	for (UInt16 i = 0; i < attachmentElement->Count; i++)
	{
		CElement_Attachment& Attachment = *attachmentElement->at<CElement_Attachment>(i);

		DataBinary.WriteUInt32(Attachment.ID);
		DataBinary.WriteSInt16(Attachment.ParentBone);
		DataBinary.WriteC3Vector(Attachment.Position);
		DataBinary.WriteFloat32(1.0f);
	}

	// write cameras
	DataBinary.WriteUInt32(CamerasCount);
	for (UInt16 i = 0; i < CamerasCount; i++)
	{
		SInt32 CameraType;
		Float32 ClipFar, ClipNear, FoV;
		C3Vector Position, Target;

		if (GetExpansion() < Expansion::Cataclysm)
		{
			auto& Camera = Elements[EElement_Camera].as<CElement_Camera_PreCata>()[i];
			CameraType = Camera.Type;
			ClipFar = Camera.ClipFar;
			ClipNear = Camera.ClipNear;
			Position = Camera.Position;
			Target = Camera.Target;
			FoV = Camera.FoV;
		}
		else
		{
			auto& Camera = Elements[EElement_Camera].as<CElement_Camera>()[i];
			CameraType = Camera.Type;
			ClipFar = Camera.ClipFar;
			ClipNear = Camera.ClipNear;
			Position = Camera.Position;
			Target = Camera.Target;

			// extract field of view of camera from animation block
			if (Camera.AnimationBlock_FieldOfView.Keys.Count > 0)
			{
				auto ExternalAnimations = (M2Array*)Elements[EElement_Camera].GetLocalPointer(Camera.AnimationBlock_FieldOfView.Keys.Offset);
				auto LastElement = GetLastElement();
				assert(LastElement != NULL);
				assert(ExternalAnimations[0].Offset >= LastElement->Offset && ExternalAnimations[0].Offset < LastElement->Offset + LastElement->Data.size());

				Float32* FieldOfView_Keys = (Float32*)LastElement->GetLocalPointer(ExternalAnimations[0].Offset);
				FoV = FieldOfView_Keys[0];
			}
			else
				FoV = 0.785398163f;	// 45 degrees in radians, assuming that WoW stores camera FoV in radians. or maybe it's half FoV.
		}

		DataBinary.WriteUInt8(1);	// has data
		DataBinary.WriteSInt32(CameraType);

		DataBinary.WriteFloat32(FoV);

		DataBinary.WriteFloat32(ClipFar);
		DataBinary.WriteFloat32(ClipNear);
		DataBinary.WriteC3Vector(Position);
		DataBinary.WriteC3Vector(Target);
	}

	FileStream.close();

	return EError_OK;
}

M2Lib::EError M2Lib::M2::ImportM2Intermediate(Char16 const* FileName)
{
	bool IgnoreBones = Settings && !Settings->MergeBones;
	bool IgnoreAttachments = Settings && !Settings->MergeAttachments;
	bool IgnoreCameras = Settings && !Settings->MergeCameras;
	bool FixSeams = !Settings || Settings->FixSeams;
	bool IgnoreOriginalMeshIndexes = Settings && Settings->IgnoreOriginalMeshIndexes;

	Float32 SubmeshPositionalTolerance = 0.0001f;
	Float32 SubmeshAngularTolerance = 45.0f;
	Float32 BodyPositionalTolerance = 0.0001f;
	Float32 BodyAngularTolerance = 90.0f;
	Float32 ClothingPositionalTolerance = 0.0001f;
	Float32 ClothingAngularTolerance = 90.0f;

	if (!FileName)
		return EError_FailedToImportM2I_NoFileSpecified;

	// check that we have an M2 already loaded and ready to be injected
	if (!Header.Elements.nSkin)
		return EError_FailedToExportM2I_M2NotLoaded;

	if (pInM2I)
		delete pInM2I;
	pInM2I = new M2I();

	auto Error = pInM2I->Load(FileName, this, IgnoreBones, IgnoreAttachments, IgnoreCameras, IgnoreOriginalMeshIndexes);
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
	std::vector< UInt16 > NewBoneLookup;
	M2Skin* pNewSkin0 = new M2Skin(this);
	assert(SkinBuilder.Build(pNewSkin0, 256, pInM2I, &NewVertexList[0], 0));

	// set skin 0 so we can begin seam fixing
	M2Skin* pOriginalSkin0 = Skins[0];	// save this because we will need to copy materials from it later.
	OriginalSkinCount = Header.Elements.nSkin;
	Header.Elements.nSkin = 1;
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		if (Skins[i])
		{
			if (i != 0)
				delete Skins[i];
			Skins[i] = NULL;
		}
	}

	Skins[0] = pNewSkin0;

	if (FixSeams)
	{
		// fix normals within submeshes
		FixSeamsSubMesh(SubmeshPositionalTolerance, SubmeshAngularTolerance * DegreesToRadians);

		// fix normals between body submeshes
		FixSeamsBody(BodyPositionalTolerance, BodyAngularTolerance * DegreesToRadians);

		// close gaps between clothes and body
		FixSeamsClothing(ClothingPositionalTolerance, ClothingAngularTolerance * DegreesToRadians);
	}

	//
	//
	//
	// build skins for real this time
	// because a few bone indices might have changed during seam and gap fixing
	// this list will store the new skins
	M2Skin* NewSkinList[SKIN_COUNT];
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
		NewSkinList[i] = NULL;

	// for easy looping
	UInt32 MaxBoneList[SKIN_COUNT - LOD_SKIN_MAX_COUNT + 1];
	MaxBoneList[0] = 256;
	MaxBoneList[1] = 64;
	MaxBoneList[2] = 53;
	MaxBoneList[3] = 21;
	MaxBoneList[4] = 0;		// extra entry needed for LoD check
	//MaxBoneList[4] = 64;	// extracted from client
	//MaxBoneList[5] = 64;
	//MaxBoneList[6] = 64;

	NewBoneLookup.clear();
	SInt32 BoneStart = 0;
	UInt32 iSkin = 0;

	for (UInt32 iLoD = 0; iLoD < SKIN_COUNT - LOD_SKIN_MAX_COUNT; ++iLoD)
	{
		M2Skin* pNewSkin = new M2Skin(this);
		assert(SkinBuilder.Build(pNewSkin, MaxBoneList[iLoD], pInM2I, Elements[EElement_Vertex].as<CVertex>(), BoneStart));
		if (iLoD == 0)
		{
			// fill extra data with mesh indexes from zero skin
			for (UInt32 i = 0; i < pNewSkin->ExtraDataBySubmeshIndex.size(); ++i)
				const_cast<SubmeshExtraData*>(pNewSkin->ExtraDataBySubmeshIndex[i])->FirstLODMeshIndex = i;
		}

		// if there are more bones than the next lowest level of detail
		if (SkinBuilder.m_Bones.size() > MaxBoneList[iLoD + 1])
		{
			// copy skin to result list
			NewSkinList[iSkin] = pNewSkin;
			++iSkin;

			// copy skin's bone lookup to the global bone lookup list
			for (UInt32 i = 0; i < SkinBuilder.m_Bones.size(); i++)
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
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		if (NewSkinList[i])
		{
			NewSkinList[i]->CopyMaterials(pOriginalSkin0);
			//NewSkinList[i]->SortSubMeshes();
		}
	}
	delete pOriginalSkin0;

	// replace old skins with new
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		if (Skins[i])
			delete Skins[i];

		Skins[i] = NewSkinList[i];
	}

	// copy new bone lookup
	Elements[EElement_SkinnedBoneLookup].SetDataSize(NewBoneLookup.size(), NewBoneLookup.size() * sizeof(UInt16), false);
	memcpy(Elements[EElement_SkinnedBoneLookup].Data.data(), &NewBoneLookup[0], NewBoneLookup.size() * sizeof(UInt16));

	// build vertex bone indices
	for (UInt32 i = 0; i < Header.Elements.nSkin; ++i)
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
	for (UInt32 i = 0; i < BOUNDING_VERTEX_COUNT; ++i)
		boundingVertices[i].Position = ExtraData.BoundingVertices[i];

	Elements[EElement_BoundingNormal].SetDataSize(BOUNDING_TRIANGLE_COUNT, sizeof(CElement_BoundingNormals) * BOUNDING_TRIANGLE_COUNT, false);
	auto boundingNormals = Elements[EElement_BoundingNormal].as<CElement_BoundingNormals>();
	for (UInt32 i = 0; i < BOUNDING_TRIANGLE_COUNT; ++i)
		boundingNormals[i].Normal = ExtraData.BoundingNormals[i];

	Elements[EElement_BoundingTriangle].SetDataSize(BOUNDING_TRIANGLE_COUNT * 3, sizeof(CElement_BoundingTriangle) * BOUNDING_TRIANGLE_COUNT * 3, false);
	auto boundingTriangles = Elements[EElement_BoundingTriangle].as<CElement_BoundingTriangle>();
	for (UInt32 i = 0; i < BOUNDING_TRIANGLE_COUNT * 3; ++i)
		boundingTriangles[i].Index = BoundaryData::ExtraData::BoundingTriangleVertexMap[i];
}

void M2Lib::M2::PrintInfo()
{
	//
	//
	// just print out any sort of data that we want to analize when troubleshooting

	UInt32 Count = 0;

	std::wstring FileOut = FileSystemW::GetParentDirectory(_FileName) + L"\\" + FileSystemW::GetFileName(_FileName) + L".txt";

	std::fstream FileStream;
	FileStream.open(FileOut.c_str(), std::ios::out | std::ios::trunc);

	FileStream << "ID       " << Header.Description.ID[0] << Header.Description.ID[1] << Header.Description.ID[2] << Header.Description.ID[3] << std::endl;		// 'MD20'
	FileStream << "Version  " << Header.Description.Version << std::endl;
	FileStream << std::endl;

	FileStream << "nName                     " << Header.Description.nName << std::endl;
	FileStream << "oName                     " << Header.Description.oName << std::endl;
	FileStream << " Value                    " << Elements[EElement_Name].as<Char8>() << std::endl;
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

    for (UInt32 i = 0; i < Header.Elements.nTexture; ++i)
    {
		CElement_Texture* texture = Elements[EElement_Texture].as<CElement_Texture>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tFlags: " << (UInt32)texture[i].Flags << std::endl;
        FileStream << "\tType: " << (UInt32)texture[i].Type << std::endl;
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
    for (UInt32 i = 0; i < Header.Elements.nTransparency; ++i)
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
            SInt16 GlobalSequenceID;
            UInt32 nTimes;
            UInt32 oTimes;
            UInt32 nKeys;
            UInt32 oKeys;
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
    for (UInt32 i = 0; i < Header.Elements.nTransparency; ++i)
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

    for (UInt32 i = 0; i < Header.Elements.nTexture; ++i)
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
	sLogger.Log("=====START REFERENCED FILE INFO=======");
	auto skinChunk = (SFIDChunk*)GetChunk(EM2Chunk::Skin);
	if (!skinChunk)
		sLogger.Log("Skins files are not referenced by M2");
	auto skeletonChunk = (SKIDChunk*)GetChunk(EM2Chunk::Skeleton);
	if (!skeletonChunk)
		sLogger.Log("Skeleton is not referenced by M2");
	auto textureChunk = (TXIDChunk*)GetChunk(EM2Chunk::Texture);
	if (!textureChunk)
		sLogger.Log("Texture files are not referenced by M2");

	auto casc = GetCasc();
	auto pathInfo = [casc](UInt32 FileDataId) -> std::string
	{
		if (!FileDataId)
			return "<none>";
		
		if (!casc)
			return "<casc not initialized>";
		
		std::string path = casc->GetFileByFileDataId(FileDataId);
		if (path.empty())
			path = "<not cached>";

		return path;
	};

	if (skinChunk)
	{
		sLogger.Log("Total skin files referenced: %u", skinChunk->SkinsFileDataIds.size());
		for (auto skinFileDataId : skinChunk->SkinsFileDataIds)
			if (skinFileDataId)
				sLogger.Log("\t[%u] %s", skinFileDataId, pathInfo(skinFileDataId).c_str());
	}
	if (skeletonChunk)
	{
		sLogger.Log("Skeleton file referenced:");
		sLogger.Log("\t[%u] %s", skeletonChunk->SkeletonFileDataId, pathInfo(skeletonChunk->SkeletonFileDataId).c_str());
		if (Skeleton)
		{
			using namespace SkeletonChunk;

			if (auto skpdChunk = (SKPDChunk*)Skeleton->GetChunk(ESkeletonChunk::SKPD))
				sLogger.Log("\tParent skeleton file: [%u] %s", skpdChunk->Data.ParentSkeletonFileId, pathInfo(skpdChunk->Data.ParentSkeletonFileId).c_str());
			else
				sLogger.Log("\tNo parent skeleton referenced");
			if (auto afidChunk = (AFIDChunk*)Skeleton->GetChunk(ESkeletonChunk::AFID))
			{
				sLogger.Log("\tTotal skeleton animation files referenced: %u", afidChunk->AnimInfos.size());
				for (auto anim : afidChunk->AnimInfos)
					sLogger.Log("\t\t[%u] %s", anim.FileId, pathInfo(anim.FileId).c_str());
			}
			if (auto boneChunk = (BFIDChunk*)Skeleton->GetChunk(ESkeletonChunk::BFID))
			{
				sLogger.Log("\tTotal skeleton bone files referenced: %u", boneChunk->BoneFileDataIds.size());
				for (auto bone : boneChunk->BoneFileDataIds)
					sLogger.Log("\t\t[%u] %s", bone, pathInfo(bone).c_str());
			}
		}
		else
			sLogger.Log("\tError! Skeleton is referenced, but not loaded");
	}
	if (textureChunk)
	{
		UInt32 count = 0;
		for (auto textuteFileDataId : textureChunk->TextureFileDataIds)
			if (textuteFileDataId)
				++count;

		sLogger.Log("Total texture files referenced: %u", count);
		for (auto textuteFileDataId : textureChunk->TextureFileDataIds)
			if (textuteFileDataId)
				sLogger.Log("\t[%u] %s", textuteFileDataId, pathInfo(textuteFileDataId).c_str());

		if (textureChunk->TextureFileDataIds.size() != Elements[EElement_Texture].Count)
			sLogger.Log("\tError: M2 texture block element count is not equal to chunk element count! (%u vs %u)", Elements[EElement_Texture].Count, textureChunk->TextureFileDataIds.size());
		else
		{
			CElement_Texture* TextureElements = Elements[EElement_Texture].as<CElement_Texture>();
			for (UInt32 i = 0; i < Elements[EElement_Texture].Count; ++i)
			{
				auto& textureElement = TextureElements[i];
				if (textureElement.Type != CElement_Texture::ETextureType::Final_Hardcoded)
					continue;

				char const* localTexturePath = NULL;
				if (textureElement.TexturePath.Offset)
					localTexturePath = (char const*)Elements[EElement_Texture].GetLocalPointer(textureElement.TexturePath.Offset);
				auto FileDataId = textureChunk->TextureFileDataIds[i];

				if (textureElement.TexturePath.Offset && FileDataId)
				{
					auto cascPath = pathInfo(FileDataId);
					sLogger.Log("\tWarning: Texture #%u file is stored in both chunk and texture element.\r\n"
						"\t\tElement path: %s\r\n"
						"\t\tChunk path: [%u] %s", i, localTexturePath, FileDataId, pathInfo(FileDataId).c_str());
				}
				else if (textureElement.TexturePath.Offset)
					sLogger.Log("\tWarning: texture #%u '%s' is referenced in texture element but not in chunk (legacy model?)", i, localTexturePath);
				else if (!textureElement.TexturePath.Offset && !FileDataId)
					sLogger.Log("\tError: texture #%u path must be present in either in element or chunk, but it is not", i);
			}
		}
	}

	if (auto physChunk = (PFIDChunk*)GetChunk(EM2Chunk::Physic))
	{
		sLogger.Log("Physics file referenced:");
		sLogger.Log("\t[%u] %s", physChunk->PhysFileId, pathInfo(physChunk->PhysFileId).c_str());
	}
	if (auto afidChunk = (AFIDChunk*)GetChunk(EM2Chunk::Animation))
	{
		sLogger.Log("Total animation files referenced: %u", afidChunk->AnimInfos.size());
		for (auto anim : afidChunk->AnimInfos)
			sLogger.Log("\t[%u] %s", anim.FileId, pathInfo(anim.FileId).c_str());
	}
	if (auto boneChunk = (BFIDChunk*)GetChunk(EM2Chunk::Bone))
	{
		sLogger.Log("Total bone files referenced: [%u]", boneChunk->BoneFileDataIds.size());
		for (auto bone : boneChunk->BoneFileDataIds)
			sLogger.Log("\t[%u] %s", bone, pathInfo(bone).c_str());
	}

	sLogger.Log("======END OF REFERENCED FILE INFO======");
}

// Gets the .skin file names.
bool M2Lib::M2::GetFileSkin(std::wstring& SkinFileNameResultBuffer, std::wstring const& M2FileName, UInt32 SkinIndex)
{
	auto skinChunk = (M2Chunk::SFIDChunk*)GetChunk(EM2Chunk::Skin);
	if (skinChunk && casc)
	{
		//sLogger.Log("Skin chunk detected");
		SInt32 chunkIndex = -1;
		if (SkinIndex < SKIN_COUNT - LOD_SKIN_MAX_COUNT)
		{
			if (SkinIndex < Header.Elements.nSkin)
				chunkIndex = SkinIndex;
		}
		else
			chunkIndex = SkinIndex + LOD_SKIN_MAX_COUNT + Header.Elements.nSkin - SKIN_COUNT;

		if (chunkIndex < 0 || chunkIndex >= skinChunk->SkinsFileDataIds.size())
		{
			//sLogger.Log("Skin #%u is not present in chunk", SkinIndex);
			return false;
		}

		auto skinFileDataId = skinChunk->SkinsFileDataIds[chunkIndex];
		auto path = casc->GetFileByFileDataId(skinFileDataId);
		if (!path.empty())
		{
			//sLogger.Log("Skin listfile entry: %s", path.c_str());
			auto SkinFileName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(FileSystemA::GetBaseName(path));
			SkinFileNameResultBuffer = FileSystemW::GetParentDirectory(M2FileName) + L"\\" + SkinFileName;
			return true;
		}

		sLogger.Log("Warning: skin FileDataId not found in listfile! Listfile is not up to date! Trying default skin name");
	}

	SkinFileNameResultBuffer.resize(1024);

	switch (SkinIndex)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			std::swprintf((wchar_t*)SkinFileNameResultBuffer.data(), L"%s\\%s0%u.skin",
				FileSystemW::GetParentDirectory(M2FileName).c_str(), FileSystemW::GetFileName(M2FileName).c_str(), SkinIndex);
			break;
		case 4:
		case 5:
		case 6:
			std::swprintf((wchar_t*)SkinFileNameResultBuffer.data(), L"%s\\%s_LOD0%u.skin",
				FileSystemW::GetParentDirectory(M2FileName).c_str(), FileSystemW::GetFileName(M2FileName).c_str(), SkinIndex - 3);
			break;
		default:
			return false;
	}

	return true;
}

bool M2Lib::M2::GetFileSkeleton(std::wstring& SkeletonFileNameResultBuffer, std::wstring const& M2FileName)
{
	auto chunk = (M2Chunk::SKIDChunk*)GetChunk(EM2Chunk::Skeleton);
	auto casc = GetCasc();
	if (!chunk || !casc)
	{
		if (!chunk)
			sLogger.Log("Skeleton chunk not found, trying default names");
		else if (!casc)
			sLogger.Log("CASC not provided, trying default names");

		SkeletonFileNameResultBuffer = FileSystemW::GetParentDirectory(M2FileName) + L"\\" + FileSystemW::GetFileName(M2FileName) + L".skel";
		return true;
	}

	auto path = casc->GetFileByFileDataId(chunk->SkeletonFileDataId);
	if (!path.empty())
	{
		sLogger.Log("Skeleton listfile entry: %s", path.c_str());
		auto SkeletonFileName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(FileSystemA::GetBaseName(path));
		SkeletonFileNameResultBuffer = FileSystemW::GetParentDirectory(M2FileName) + L"\\" + SkeletonFileName;
		return true;
	}
	
	sLogger.Log("Warning: skeleton FileDataId not found in listfile! Listfile is not up to date! Trying default skeleton name");
	SkeletonFileNameResultBuffer = FileSystemW::GetParentDirectory(M2FileName) + L"\\" + FileSystemW::GetFileName(M2FileName) + L".skel";
	return true;
}

void M2Lib::M2::FixSeamsSubMesh(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// gather up sub meshes
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > SubMeshes;

	M2SkinElement::CElement_SubMesh* Subsets = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	UInt32 SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		UInt16 ThisID = Subsets[i].ID;
		bool MakeNew = true;
		for (UInt32 j = 0; j < SubMeshes.size(); j++)
		{
			for (UInt32 k = 0; k < SubMeshes[j].size(); k++)
			{
				if (SubMeshes[j][k]->ID == ThisID)
				{
					MakeNew = false;
					SubMeshes[j].push_back(&Subsets[i]);
					break;
				}
			}
			if (!MakeNew)
			{
				break;
			}
		}
		if (MakeNew)
		{
			std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
			NewSubmeshSubsetList.push_back(&Subsets[i]);
			SubMeshes.push_back(NewSubmeshSubsetList);
		}
	}

	// find and merge duplicate vertices
	UInt32 VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (UInt32 iSubMesh1 = 0; iSubMesh1 < SubMeshes.size(); iSubMesh1++)
	{
		for (UInt32 iSubSet1 = 0; iSubSet1 < SubMeshes[iSubMesh1].size(); iSubSet1++)
		{
			M2SkinElement::CElement_SubMesh* pSubSet1 = SubMeshes[iSubMesh1][iSubSet1];

			UInt32 VertexAEnd = pSubSet1->VertexStart + pSubSet1->VertexCount;
			for (UInt32 iVertexA = pSubSet1->VertexStart; iVertexA < VertexAEnd; iVertexA++)
			{
				bool AddedVertexA = false;
				for (UInt32 iSubSet2 = 0; iSubSet2 < SubMeshes[iSubMesh1].size(); iSubSet2++)
				{
					M2SkinElement::CElement_SubMesh* pSubSet2 = SubMeshes[iSubMesh1][iSubSet2];

					UInt32 VertexBEnd = pSubSet2->VertexStart + pSubSet2->VertexCount;
					for (UInt32 iVertexB = pSubSet2->VertexStart; iVertexB < VertexBEnd; iVertexB++)
					{
						if (iVertexA == iVertexB)
						{
							continue;
						}

						if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, PositionalTolerance, AngularTolerance))
						{
							if (!AddedVertexA)
							{
								SimilarVertices.push_back(&VertexList[iVertexA]);
								AddedVertexA = true;
							}

							SimilarVertices.push_back(&VertexList[iVertexB]);
						}
					}
				}

				if (SimilarVertices.size() > 1)
				{
					// sum positions and normals
					C3Vector NewPosition, NewNormal;

					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition = NewPosition + pSimilarVertex->Position;
						NewNormal = NewNormal + pSimilarVertex->Normal;
					}

					// average position and normalize normal
					Float32 invSimilarCount = 1.f / (Float32)SimilarVertices.size();

					NewPosition = NewPosition * invSimilarCount;
					NewNormal = NewNormal * invSimilarCount;

					UInt8 NewBoneWeights[4], NewBoneIndices[4];
					for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
					{
						NewBoneWeights[i] = SimilarVertices[0]->BoneWeights[i];
						NewBoneIndices[i] = SimilarVertices[0]->BoneIndices[i];
					}

					// assign new values back into similar vertices
					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position = NewPosition;
						pSimilarVertex->Normal = NewNormal;

						for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pSimilarVertex->BoneWeights[i] = NewBoneWeights[i];
							pSimilarVertex->BoneIndices[i] = NewBoneIndices[i];
						}
					}

					SimilarVertices.clear();
				}
			}
		}
	}
}


void M2Lib::M2::FixSeamsBody(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// sub meshes that are divided up accross multiple bone partitions will have multiple sub mesh entries with the same ID in the M2.
	// we need to gather each body submesh up into a list and average normals of vertices that are similar between other sub meshes.
	// this function is designed to be used on character models, so it may not work on other models.

	// list of submeshes that make up the body of the character
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > CompiledSubMeshList;

	// gather up the body submeshes
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	UInt32 SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		// determine type of subset
		UInt16 ThisID = SubMeshList[i].ID;
		UInt16 Mod = ThisID;
		while (Mod > 10)
		{
			Mod %= 10;
		}
		if ((ThisID == 0) || (ThisID > 10 && Mod == 1) || (ThisID == 702))
		{
			// this subset is part of the character's body
			// add it to the list of submeshes
			bool MakeNew = true;
			for (UInt32 j = 0; j < CompiledSubMeshList.size(); j++)
			{
				for (UInt32 k = 0; k < CompiledSubMeshList[j].size(); k++)
				{
					if (CompiledSubMeshList[j][k]->ID == ThisID)
					{
						MakeNew = false;
						CompiledSubMeshList[j].push_back(&SubMeshList[i]);
						break;
					}
				}
				if (!MakeNew)
				{
					break;
				}
			}
			if (MakeNew)
			{
				std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
				NewSubmeshSubsetList.push_back(&SubMeshList[i]);
				CompiledSubMeshList.push_back(NewSubmeshSubsetList);
			}
		}
	}

	// find and merge duplicate vertices
	UInt32 VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (SInt32 iSubMesh1 = 0; iSubMesh1 < (SInt32)CompiledSubMeshList.size() - 1; iSubMesh1++)
	{
		for (SInt32 iSubSet1 = 0; iSubSet1 < (SInt32)CompiledSubMeshList[iSubMesh1].size(); iSubSet1++)
		{
			// gather duplicate vertices
			// for each vertex in the subset, compare it against vertices in the other subsets
			// find duplicates and sum their normals
			UInt32 iVertexAEnd = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart + CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexCount;
			for (UInt32 iVertexA = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart; iVertexA < iVertexAEnd; iVertexA++)
			{
				// gather duplicate vertices from other submeshes
				bool AddedVertex1 = false;
				for (SInt32 iSubMesh2 = iSubMesh1 + 1; iSubMesh2 < (SInt32)CompiledSubMeshList.size(); iSubMesh2++)
				{
					// check that we don't check against ourselves
					if (iSubMesh2 == iSubMesh1)
					{
						// other submesh is same as this submesh
						continue;
					}
					// go through subsets
					for (SInt32 iSubSet2 = 0; iSubSet2 < (SInt32)CompiledSubMeshList[iSubMesh2].size(); iSubSet2++)
					{
						// go through vertices in subset
						UInt32 iVertexBEnd = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart + CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexCount;
						for (UInt32 iVertexB = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart; iVertexB < iVertexBEnd; iVertexB++)
						{
							if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, PositionalTolerance, AngularTolerance))
							{
								// found a duplicate
								if (!AddedVertex1)
								{
									SimilarVertices.push_back(&VertexList[iVertexA]);
									AddedVertex1 = true;
								}
								// add the vertex from the other sub mesh to the list of similar vertices
								SimilarVertices.push_back(&VertexList[iVertexB]);
							}
						}
					}
				}

				// average normals of similar vertices
				if (SimilarVertices.size())
				{
					// sum positions and normals
					C3Vector NewPosition, NewNormal;

					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition = NewPosition + pSimilarVertex->Position;
						NewNormal = NewNormal + pSimilarVertex->Normal;
					}

					// average position and normalize normal
					Float32 invSimilarCount = 1.0f / (Float32)SimilarVertices.size();

					NewPosition = NewPosition * invSimilarCount;
					NewNormal = NewNormal * invSimilarCount;

					UInt8 NewBoneWeights[4], NewBoneIndices[4];
					for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
					{
						NewBoneWeights[i] = SimilarVertices[0]->BoneWeights[i];
						NewBoneIndices[i] = SimilarVertices[0]->BoneIndices[i];
					}

					// assign new values back into similar vertices
					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position = NewPosition;
						pSimilarVertex->Normal = NewNormal;

						for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pSimilarVertex->BoneWeights[i] = NewBoneWeights[i];
							pSimilarVertex->BoneIndices[i] = NewBoneIndices[i];
						}
					}

					// clear list
					SimilarVertices.clear();
				}
			}
		}
	}
}

void M2Lib::M2::FixSeamsClothing(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();

	UInt32 SubMeshListLength = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshBodyList;	// gathered body sub meshes
	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshGarbList;	// gathered clothing sub meshes

	for (UInt32 i = 0; i < SubMeshListLength; i++)
	{
		UInt16 ThisID = SubMeshList[i].ID;

		// gather body sub meshes
		UInt16 Mod = ThisID;
		if (Mod > 10)
		{
			Mod %= 10;
		}
		if (ThisID == 0 || (ThisID > 10 && Mod == 1))
		{
			SubMeshBodyList.push_back(&SubMeshList[i]);
		}
		// gather clothing sub meshes
		else if (
			ThisID == 402 ||	// cloth glove
			ThisID == 403 ||	// leather glove
			ThisID == 404 ||	// plate glove
			ThisID == 802 ||	// straight sleeve
			ThisID == 803 ||	// shaped sleeve
			ThisID == 902 ||	// low pant
			ThisID == 903 ||	// hight pant
			ThisID == 502 ||	// cloth boot
			ThisID == 503 ||	// leather boot
			ThisID == 504 ||	// plate boot
			ThisID == 505 ||	// plate boot 2
			ThisID == 1002 ||	// shirt frill short
			ThisID == 1102 ||	// shirt frill long
			ThisID == 1104 ||	// plate leg
			ThisID == 1202 ||	// tabard
			ThisID == 1302		// skirt
			//ThisID == 1802	// plate belt
			)
		{
			SubMeshGarbList.push_back(&SubMeshList[i]);
		}
	}

	// copy vertex properties from main body vertex to duplicate clothing vertices
	for (UInt32 iSubMeshGarb = 0; iSubMeshGarb < SubMeshGarbList.size(); iSubMeshGarb++)
	{
		M2SkinElement::CElement_SubMesh* pSubMeshGarb = SubMeshGarbList[iSubMeshGarb];
		for (UInt32 iSubMeshBody = 0; iSubMeshBody < SubMeshBodyList.size(); iSubMeshBody++)
		{
			M2SkinElement::CElement_SubMesh* pSubMeshBody = SubMeshBodyList[iSubMeshBody];

			for (SInt32 iVertexGarb = pSubMeshGarb->VertexStart; iVertexGarb < pSubMeshGarb->VertexStart + pSubMeshGarb->VertexCount; iVertexGarb++)
			{
				for (SInt32 iVertexBody = pSubMeshBody->VertexStart; iVertexBody < pSubMeshBody->VertexStart + pSubMeshBody->VertexCount; iVertexBody++)
				{
					if (CVertex::CompareSimilar(VertexList[iVertexGarb], VertexList[iVertexBody], false, false, PositionalTolerance, AngularTolerance))
					{
						// copy position, normal, and bone weights, and bone indices from body vertex to other(clothing) vertex
						CVertex* pVertexOther = &VertexList[iVertexGarb];
						CVertex* pVertexBody = &VertexList[iVertexBody];

						pVertexOther->Position = pVertexBody->Position;
						pVertexOther->Normal = pVertexBody->Normal;

						for (UInt32 i = 0; i < BONES_PER_VERTEX; ++i)
						{
							pVertexOther->BoneWeights[i] = pVertexBody->BoneWeights[i];
							pVertexOther->BoneIndices[i] = pVertexBody->BoneIndices[i];
						}
					}
				}
			}
		}
	}
}

void M2Lib::M2::Scale(Float32 Scale)
{
	// vertices
	{
		UInt32 VertexListLength = Elements[EElement_Vertex].Count;
		CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
		for (UInt32 i = 0; i < VertexListLength; i++)
		{
			CVertex& Vertex = VertexList[i];
			Vertex.Position = Vertex.Position * Scale;
		}
	}

	// bones
	{
		auto boneElement = GetBones();
		UInt32 BoneListLength = boneElement->Count;
		CElement_Bone* BoneList = boneElement->as<CElement_Bone>();
		for (UInt32 i = 0; i < BoneListLength; i++)
		{
			CElement_Bone& Bone = BoneList[i];
			Bone.Position = Bone.Position * Scale;
		}
	}

	// attachments
	{
		auto attachmentElement = GetAttachments();
		UInt32 AttachmentListLength = attachmentElement->Count;
		CElement_Attachment* AttachmentList = attachmentElement->as<CElement_Attachment>();
		for (UInt32 i = 0; i < AttachmentListLength; i++)
		{
			CElement_Attachment& Attachment = AttachmentList[i];
			Attachment.Position = Attachment.Position * Scale;
		}
	}

	// events
	{
		UInt32 EventListLength = Elements[EElement_Event].Count;
		CElement_Event* EventList = Elements[EElement_Event].as<CElement_Event>();
		for (UInt32 i = 0; i < EventListLength; i++)
		{
			CElement_Event& Event = EventList[i];
			Event.Position = Event.Position * Scale;
		}
	}

	// lights
	{
		UInt32 LightListLength = Elements[EElement_Light].Count;
		CElement_Light* LightList = Elements[EElement_Light].as<CElement_Light>();
		for (UInt32 i = 0; i < LightListLength; i++)
		{
			CElement_Light& Light = LightList[i];
			Light.Position = Light.Position * Scale;
		}
	}

	// cameras
	{
		UInt32 CameraListLength = Elements[EElement_Camera].Count;
		CElement_Camera* CameraList = Elements[EElement_Camera].as<CElement_Camera>();
		for (UInt32 i = 0; i < CameraListLength; i++)
		{
			CElement_Camera& Camera = CameraList[i];
			Camera.Position = Camera.Position * Scale;
			Camera.Target = Camera.Target * Scale;
		}
	}

	// ribbon emitters
	{
		UInt32 RibbonEmitterListLength = Elements[EElement_RibbonEmitter].Count;
		CElement_RibbonEmitter* RibbonEmitterList = Elements[EElement_RibbonEmitter].as<CElement_RibbonEmitter>();
		for (UInt32 i = 0; i < RibbonEmitterListLength; i++)
		{
			CElement_RibbonEmitter& RibbonEmitter = RibbonEmitterList[i];
			RibbonEmitter.Position = RibbonEmitter.Position * Scale;
		}
	}

	// particle emitters
	{
		UInt32 ParticleEmitterListLength = Elements[EElement_ParticleEmitter].Count;
		CElement_ParticleEmitter* ParticleEmitterList = Elements[EElement_ParticleEmitter].as<CElement_ParticleEmitter>();
		for (UInt32 i = 0; i < ParticleEmitterListLength; i++)
		{
			CElement_ParticleEmitter& ParticleEmitter = ParticleEmitterList[i];
			ParticleEmitter.Position = ParticleEmitter.Position * Scale;
		}
	}
}


void M2Lib::M2::MirrorCamera()
{
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();
	UInt32 CameraCount = Elements[EElement_Camera].Count;
	for (UInt32 iCamera = 0; iCamera < CameraCount; iCamera++)
	{
		if (Cameras->Type == 0)
		{
			Cameras->Position.Y = -Cameras->Position.Y;
			Cameras->Target.Y = -Cameras->Target.Y;
			break;
		}
	}

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


void M2Lib::M2::m_LoadElements_FindSizes(UInt32 ChunkSize)
{
	for (UInt32 i = 0; i < EElement__Count__; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = ChunkSize;
		for (UInt32 j = 0; j < EElement__Count__; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		assert(NextOffset >= Element.Offset && "M2 Elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}
}

#define IS_LOCAL_ELEMENT_OFFSET(offset) \
	(!offset || Elements[iElement].Offset <= offset && offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size())
#define VERIFY_OFFSET_LOCAL( offset ) \
	assert(IS_LOCAL_ELEMENT_OFFSET(offset));
#define VERIFY_OFFSET_NOTLOCAL( offset ) \
	assert( !offset || offset >= Elements[iElement].OffsetOriginal + Elements[iElement].Data.size() );

void M2Lib::M2::SetCasc(Casc * casc)
{
	this->casc = casc;
	casc->AddRefence();
}

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
	SInt32 CurrentOffset = 0;
	if (Header.IsLongHeader() && GetExpansion() >= Expansion::Cataclysm)
		CurrentOffset = sizeof(CM2Header) + 8;	// +8 to align data at 16 byte bounds
	else
		CurrentOffset = sizeof(CM2Header) - 8;	// -8 because last 2 UInt32s are not saved

	// totaldiff needed to fix animations that are in the end of a chunk
	SInt32 totalDiff = -(SInt32)m_OriginalModelChunkSize + GetHeaderSize();
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
		totalDiff += Elements[iElement].Data.size();

	SInt32 OffsetDelta = 0;
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Opacity, iElement);
				}
				break;
			}

			case EElement_Texture:
			{
				CElement_Texture* Textures = Elements[iElement].as<CElement_Texture>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Transparencies[j].AnimationBlock_Transparency, iElement);
				}
				break;
			}
			case EElement_TextureAnimation:
			{
				CElement_UVAnimation* Animations = Elements[iElement].as<CElement_UVAnimation>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
					m_FixAnimationM2Array(OffsetDelta, totalDiff, Events[j].GlobalSequenceID, Events[j].TimeLines, iElement);

				break;
			}

			case EElement_Light:
			{
				CElement_Light* Lights = Elements[iElement].as<CElement_Light>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
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
			sLogger.Log("Element #%u size changed", iElement);
		Elements[iElement].SizeOriginal = Elements[iElement].Data.size();
		Elements[iElement].OffsetOriginal = CurrentOffset;
		CurrentOffset += Elements[iElement].Data.size();
	}

	m_OriginalModelChunkSize = GetHeaderSize();
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
		m_OriginalModelChunkSize += Elements[iElement].Data.size();
}

void M2Lib::M2::m_FixAnimationM2Array_Old(SInt32 OffsetDelta, SInt32 TotalDelta, SInt16 GlobalSequenceID, M2Array& Array, SInt32 iElement)
{
#define IS_LOCAL_ANIMATION(Offset) \
	(Offset >= Elements[iElement].OffsetOriginal && (Offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size()))

	auto animationElement = GetAnimations();
	assert("Failed to get model animations" && animationElement);
	assert("Zero animations count for model" && animationElement->Count);

	auto animations = animationElement->as<CElement_Animation>();

	VERIFY_OFFSET_LOCAL(Array.Offset);

	if (GlobalSequenceID == -1)
	{
		if (Array.Count)
		{
			auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);

			for (UInt32 i = 0; i < Array.Count; ++i)
			{
				assert("Out of animation index" && i < animationElement->Count);
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
		for (UInt32 i = 0; i < Array.Count; ++i)
			SubArrays[i].Shift(IS_LOCAL_ANIMATION(SubArrays[i].Offset) ? OffsetDelta : TotalDelta);

		Array.Shift(IS_LOCAL_ANIMATION(Array.Offset) ? OffsetDelta : TotalDelta);
	}
}

void M2Lib::M2::m_FixAnimationM2Array(SInt32 OffsetDelta, SInt32 TotalDelta, SInt16 GlobalSequenceID, M2Array& Array, SInt32 iElement)
{
	if (!Array.Count)
		return;

	auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);
	auto& Element = Elements[iElement];

	for (UInt32 i = 0; i < Array.Count; ++i)
	{
		if (!SubArrays[i].Offset)
			continue;

		assert("i < animation.count" && i < GetAnimations()->Count);
		auto animation = GetAnimations()->at<CElement_Animation>(i);

		/*auto lte = SubArrays[i].Offset < Element.Offset ? 1 : 0;
		auto ext = SubArrays[i].Offset > (UInt32)Element.Data.size() + Element.Offset ? 1 : 0;
		auto flags = (animation->Flags & 0x20) != 0 ? 1 : 0;
		if ((lte == 0 && ext == 1 && flags == 0 || i == 68) && iElement == EElement_Transparency)
		{
			int a = 1;
		}
		sLogger.Log("lte:%u ext:%u seq:%i flags:%X flags&20:%u",
			lte,
			ext,
			GlobalSequenceID,
			animation->Flags,
			flags);*/

		if (animation->IsInline())
		{
			assert("Not external offset" && SubArrays[i].Offset > Elements[iElement].Offset + Elements[iElement].SizeOriginal);
			SubArrays[i].Shift(TotalDelta);
		}
	}

	Array.Shift(OffsetDelta);
}

void M2Lib::M2::m_FixAnimationOffsets_Old(SInt32 OffsetDelta, SInt32 TotalDelta, CElement_AnimationBlock& AnimationBlock, SInt32 iElement)
{
	m_FixAnimationM2Array_Old(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array_Old(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixAnimationOffsets(SInt32 OffsetDelta, SInt32 TotalDelta, CElement_AnimationBlock& AnimationBlock, SInt32 iElement)
{
	if (Settings && Settings->FixAnimationsTest)
	{
		if (AnimationBlock.Times.Count != AnimationBlock.Keys.Count)
			return;
	}

	//auto animationElement = GetAnimations();
	//assert("count(anims) != M2Array.Count" && AnimationBlock.Times.Count == animationElement->Count);
	//sLogger.Log("seq:%i count:%u anims:%u eq:%u", AnimationBlock.GlobalSequenceID, AnimationBlock.Times.Count,animationElement->Count, AnimationBlock.Times.Count == animationElement->Count ? 1 :0);

	m_FixAnimationM2Array(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixFakeAnimationBlockOffsets(SInt32 OffsetDelta, SInt32 TotalDelta, CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement)
{
	if (Settings && Settings->FixAnimationsTest)
	{
		if (AnimationBlock.Times.Count != AnimationBlock.Keys.Count)
			return;
	}

	//auto animationElement = GetAnimations();
	//assert("count(anims) != M2Array.Count" && AnimationBlock.Times.Count == animationElement->Count);
	//sLogger.Log("seq:%i count:%u anims:%u eq:%u", AnimationBlock.GlobalSequenceID, AnimationBlock.Times.Count,animationElement->Count, AnimationBlock.Times.Count == animationElement->Count ? 1 :0);

	m_FixAnimationM2Array(OffsetDelta, TotalDelta, -1, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array(OffsetDelta, TotalDelta, -1, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixFakeAnimationBlockOffsets_Old(SInt32 OffsetDelta, SInt32 TotalDelta, CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement)
{
	// TP is the best
	if (AnimationBlock.Times.Count)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.Times.Offset);

		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.Times.Offset) && (AnimationBlock.Times.Offset < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.Times.Offset);
		assert(AnimationBlock.Times.Offset > 0);
		AnimationBlock.Times.Offset += OffsetDelta;
	}

	if (AnimationBlock.Keys.Count)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.Keys.Offset);
		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.Keys.Offset) && (AnimationBlock.Keys.Offset < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.Keys.Offset);
		assert(AnimationBlock.Keys.Offset > 0);
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

UInt32 M2Lib::M2::AddTexture(const Char8* szTextureSource, CElement_Texture::ETextureType Type, CElement_Texture::ETextureFlags Flags)
{
	auto& Element = Elements[EElement_Texture];

	// shift offsets for existing textures
	for (UInt32 i = 0; i < Element.Count; ++i)
	{
		auto& texture = Element.as<CElement_Texture>()[i];
		if (texture.TexturePath.Offset)
			texture.TexturePath.Offset += sizeof(CElement_Texture);
	}

	// add element placeholder for new texture
	Element.Data.insert(Element.Data.begin() + Element.Count * sizeof(CElement_Texture), sizeof(CElement_Texture), 0);
	
	bool rawPathTexture = true;
	auto casc = GetCasc();
	auto textureChunk = (TXIDChunk*)GetChunk(EM2Chunk::Texture);
	if (casc && textureChunk)
	{
		if (auto FileDataId = casc->GetFileDataIdByFile(szTextureSource))
		{
			rawPathTexture = false;
			textureChunk->TextureFileDataIds.push_back(FileDataId);
		}
	}

	if (rawPathTexture && textureChunk)
	{
		sLogger.Log("Texture %s is not indexed in CASC, TXID chunk will be removed from model", szTextureSource);
		textureChunk->TextureFileDataIds.push_back(0);
		// texture is not indexed, texture chunk needs to be removed
		needRemoveTXIDChunk = true;
	}
	
	auto texturePathPos = Element.Data.size();

	if (rawPathTexture)
	{
		// add placeholder for texture path
		Element.Data.insert(Element.Data.end(), strlen(szTextureSource) + 1, 0);
	}

	auto newIndex = Element.Count;

	CElement_Texture& newTexture = Element.as<CElement_Texture>()[newIndex];
	newTexture.Type = Type;
	newTexture.Flags = Flags;
	newTexture.TexturePath.Count = rawPathTexture ? strlen(szTextureSource) + 1 : 0;
	newTexture.TexturePath.Offset = rawPathTexture ? Element.Offset + texturePathPos : 0;

	if (rawPathTexture)
		memcpy(&Element.Data[texturePathPos], szTextureSource, newTexture.TexturePath.Count);

	++Element.Count;

	return newIndex;
}

UInt32 M2Lib::M2::CloneTexture(UInt16 TextureId)
{
	assert(TextureId < Header.Elements.nTexture && "Too large texture index");

	auto& texture = Elements[EElement_Texture].as<CElement_Texture>()[TextureId];
	std::string texturePath = (char*)Elements[EElement_Texture].GetLocalPointer(texture.TexturePath.Offset);

	return AddTexture(texturePath.c_str(), texture.Type, texture.Flags);
}

UInt32 M2Lib::M2::AddTextureFlags(CElement_TextureFlag::EFlags Flags, CElement_TextureFlag::EBlend Blend)
{
	auto& Element = Elements[EElement_TextureFlags];
	auto newIndex = Element.Count;

	Element.Data.insert(Element.Data.end(), sizeof(CElement_TextureFlag), 0);
	CElement_TextureFlag& newFlags = Element.as<CElement_TextureFlag>()[newIndex];
	newFlags.Flags = Flags;
	newFlags.Blend = Blend;

	++Element.Count;
	return newIndex;
}

UInt32 M2Lib::M2::GetTextureIndex(const Char8* szTextureSource)
{
	if (auto textureChunk = (M2Chunk::TXIDChunk*)GetChunk(EM2Chunk::Texture))
	{
		if (casc)
		{
			// if file not found - can be custom texture
			if (auto FileDataId = casc->GetFileDataIdByFile(szTextureSource))
			{
				for (UInt32 i = 0; i < textureChunk->TextureFileDataIds.size(); ++i)
					if (textureChunk->TextureFileDataIds[i] == FileDataId)
						return i;
			}
		}
	}

	auto& Element = Elements[EElement_Texture];
	for (UInt32 i = 0; i < Element.Count; ++i)
	{
		auto& texture = Element.as<CElement_Texture>()[i];
		if (texture.TexturePath.Offset)
		{
			auto pTexturePath = (Char8 const*)Element.GetLocalPointer(texture.TexturePath.Offset);
			if (Casc::CalculateHash(pTexturePath) == Casc::CalculateHash(szTextureSource))
				return i;
		}
	}

	return -1;
}

std::string M2Lib::M2::GetTexturePath(UInt32 Index)
{
	auto& TextureElement = Elements[EElement_Texture];
	assert("Texture index too large" && Index < TextureElement.Count);

	auto texture = TextureElement.at<CElement_Texture>(Index);

	if (texture->Type != M2Element::CElement_Texture::ETextureType::Final_Hardcoded)
		return "";

	if (texture->TexturePath.Offset && texture->TexturePath.Count)
		return (char*)TextureElement.GetLocalPointer(texture->TexturePath.Offset);

	auto textureChunk = (TXIDChunk*)GetChunk(EM2Chunk::Texture);
	auto casc = GetCasc();
	if (!textureChunk || !casc || textureChunk->TextureFileDataIds.size() <= Index)
		return "";

	return casc->GetFileByFileDataId(textureChunk->TextureFileDataIds[Index]);
}

void M2Lib::M2::RemoveTXIDChunk()
{
	auto casc = GetCasc();
	if (!casc)
		return;

	sLogger.Log("Erasing TXID chunk from model");

	auto chunkItr = Chunks.find(EM2Chunk::Texture);
	if (chunkItr == Chunks.end())
	{
		sLogger.Log("TXID chunk not found");
		return;
	}

	auto chunk = (M2Chunk::TXIDChunk*)chunkItr->second;

	UInt32 newDataLen = 0;
	std::map<UInt32, std::string> PathsByTextureId;

	auto& Element = Elements[EElement_Texture];
	for (UInt32 i = 0; i < chunk->TextureFileDataIds.size(); ++i)
	{
		assert(i < Element.Count);
		auto FileDataId = chunk->TextureFileDataIds[i];
		if (!FileDataId)
			continue;

		auto path = casc->GetFileByFileDataId(FileDataId);
		if (!path.empty())
		{
			sLogger.Log("Error: failed to get path for FileDataId = [%u] for texture #%u. Casc is not initialized or listfile is not loaded or not up to date", FileDataId, i);
			sLogger.Log("Custom textures will not be loaded");
			return;
		}

		PathsByTextureId[i] = path;
		newDataLen += path.length() + 1;
	}

	sLogger.Log("Total data for storing textures needed: %u", newDataLen);

	if (!newDataLen)
		return;

	UInt32 pathOffset = 0;
	UInt32 OldSize = Element.Data.size();
	Element.Data.insert(Element.Data.end(), newDataLen, 0);
	for (auto itr : PathsByTextureId)
	{
		auto texture = Element.at<CElement_Texture>(itr.first);
		texture->TexturePath.Offset = Element.Offset + OldSize + pathOffset;
		texture->TexturePath.Count = itr.second.length() + 1;

		memcpy(&Element.Data[OldSize + pathOffset], itr.second.data(), itr.second.length());
		pathOffset += itr.second.length() + 1;
	}

	sLogger.Log("Moved %u textures from chunk", PathsByTextureId.size());

	Chunks.erase(chunkItr);
}

UInt32 M2Lib::M2::AddTextureLookup(UInt16 TextureId, bool ForceNewIndex /*= false*/)
{
	auto& Element = Elements[EElement_TextureLookup];

	if (!ForceNewIndex)
	{
		for (UInt32 i = 0; i < Element.Count; ++i)
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

UInt32 M2Lib::M2::AddBone(CElement_Bone const& Bone)
{
	auto& BoneElement = Elements[M2Element::EElement_Bone];
	auto newBoneId = BoneElement.Count;

	auto Bones = Elements[M2Element::EElement_Bone].as<CElement_Bone>();
	for (UInt32 i = 0; i < BoneElement.Count; ++i)
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

