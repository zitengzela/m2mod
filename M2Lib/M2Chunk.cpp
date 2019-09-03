#include "M2Chunk.h"
#include "M2.h"
#include <fstream>
#include <assert.h>
#include <algorithm>

void M2Lib::M2Chunk::PFIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	m2lib_assert(Size == 4 && "Bad PFID chunk size");

	FileStream.read((char*)&PhysFileId, 4);
}

void M2Lib::M2Chunk::PFIDChunk::Save(std::fstream& FileStream)
{
	FileStream.write((char*)&PhysFileId, 4);
}

void M2Lib::M2Chunk::SFIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	m2lib_assert((Size % 4) == 0 && "Bad SFID chunk size");

	SkinsFileDataIds.resize(Size / 4);
	for (uint32_t i = 0; i < Size / 4; ++i)
		FileStream.read((char*)&SkinsFileDataIds[i], 4);
}

void M2Lib::M2Chunk::SFIDChunk::Save(std::fstream& FileStream)
{
	for (uint32_t i = 0; i < SkinsFileDataIds.size(); ++i)
		FileStream.write((char*)&SkinsFileDataIds[i], 4);
}

void M2Lib::M2Chunk::AFIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t offs = 0;
	while (offs < Size)
	{
		AnimFileInfo info;
		FileStream.read((char*)&info.AnimId, 2);
		FileStream.read((char*)&info.SubAnimId, 2);
		FileStream.read((char*)&info.FileId, 4);

		AnimInfos.push_back(info);

		offs += sizeof(AnimFileInfo);
	}
}

void M2Lib::M2Chunk::AFIDChunk::Save(std::fstream& FileStream)
{
	for (auto& info : AnimInfos)
	{
		FileStream.write((char*)&info.AnimId, 2);
		FileStream.write((char*)&info.SubAnimId, 2);
		FileStream.write((char*)&info.FileId, 4);
	}
}

void M2Lib::M2Chunk::BFIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t offs = 0;
	while (offs < Size)
	{
		uint32_t boneFileDataId;
		FileStream.read((char*)&boneFileDataId, 4);
		BoneFileDataIds.push_back(boneFileDataId);

		offs += sizeof(uint32_t);
	}
}

void M2Lib::M2Chunk::BFIDChunk::Save(std::fstream& FileStream)
{
	for (auto& boneFileDataId : BoneFileDataIds)
		FileStream.write((char*)&boneFileDataId, 4);
}

void M2Lib::M2Chunk::SKIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	FileStream.read((char*)&SkeletonFileDataId, 4);
}

void M2Lib::M2Chunk::SKIDChunk::Save(std::fstream& FileStream)
{
	FileStream.write((char*)&SkeletonFileDataId, 4);
}

void M2Lib::M2Chunk::MD21Chunk::Save(std::fstream& FileStream)
{
	m2lib_assert(false && "Not implemented");
}

void M2Lib::M2Chunk::TXIDChunk::Load(std::fstream & FileStream, uint32_t Size)
{
	m2lib_assert((Size % 4) == 0 && "Bad SFID chunk size");

	TextureFileDataIds.resize(Size / 4);
	for (uint32_t i = 0; i < Size / 4; ++i)
		FileStream.read((char*)&TextureFileDataIds[i], 4);
}

void M2Lib::M2Chunk::TXIDChunk::Save(std::fstream & FileStream)
{
	for (uint32_t i = 0; i < TextureFileDataIds.size(); ++i)
		FileStream.write((char*)&TextureFileDataIds[i], 4);
}

M2Lib::M2Chunk::TXACChunk::TXACChunk(uint32_t TextureFlagsCount, uint32_t ParticleEmitterCount)
{
	TextureFlagsAC.resize(TextureFlagsCount);
	ParticleEmitterAC.resize(ParticleEmitterCount);
}

void M2Lib::M2Chunk::TXACChunk::Load(std::fstream & FileStream, uint32_t Size)
{
	m2lib_assert(Size == (TextureFlagsAC.size() + ParticleEmitterAC.size()) * sizeof(texture_ac) && "Bad TXAC chunk size");

	for (uint32_t i = 0; i < TextureFlagsAC.size(); ++i)
		FileStream.read((char*)&TextureFlagsAC[i], sizeof(texture_ac));
	for (uint32_t i = 0; i < ParticleEmitterAC.size(); ++i)
		FileStream.read((char*)&ParticleEmitterAC[i], sizeof(texture_ac));
}

void M2Lib::M2Chunk::TXACChunk::Save(std::fstream & FileStream)
{
	for (uint32_t i = 0; i < TextureFlagsAC.size(); ++i)
		FileStream.write((char*)&TextureFlagsAC[i], sizeof(texture_ac));
	for (uint32_t i = 0; i < ParticleEmitterAC.size(); ++i)
		FileStream.write((char*)&ParticleEmitterAC[i], sizeof(texture_ac));
}
