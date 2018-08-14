#include "M2Chunk.h"
#include "M2.h"
#include <fstream>
#include <assert.h>
#include <algorithm>

void M2Lib::M2Chunk::PFIDChunk::Load(std::fstream& FileStream, UInt32 Size)
{
	assert(Size == 4 && "Bad PFID chunk size");

	FileStream.read((char*)PhysFileId, 4);
}

void M2Lib::M2Chunk::PFIDChunk::Save(std::fstream& FileStream)
{
	FileStream.write((char*)&PhysFileId, 4);
}

M2Lib::M2Chunk::SFIDChunk::SFIDChunk(UInt32 SkinCount)
{
	SkinsFileDataIds.resize(SkinCount);
}

void M2Lib::M2Chunk::SFIDChunk::Load(std::fstream& FileStream, UInt32 Size)
{
	assert((Size % 4) == 0 && "Bad SFID chunk size");
	assert(SkinsFileDataIds.size() * 4 <= Size && "Bad SFID chunk size");

	for (UInt32 i = 0; i < SkinsFileDataIds.size(); ++i)
		FileStream.read((char*)&SkinsFileDataIds[i], 4);

	Lod_SkinsFileDataIds.clear();
	int RemainingBytes = Size - SkinsFileDataIds.size() * 4;
	int LodSkinFileCount = RemainingBytes ? std::min(RemainingBytes / 4, LOD_SKIN_MAX_COUNT) : 0;
	for (UInt32 i = 0; i < LodSkinFileCount; ++i)
	{
		UInt32 FileDataId = 0;
		FileStream.read((char*)&FileDataId, 4);
		Lod_SkinsFileDataIds.push_back(FileDataId);
	}
}

void M2Lib::M2Chunk::SFIDChunk::Save(std::fstream& FileStream)
{
	for (UInt32 i = 0; i < SkinsFileDataIds.size(); ++i)
		FileStream.write((char*)&SkinsFileDataIds[i], 4);
	for (UInt32 i = 0; i < Lod_SkinsFileDataIds.size(); ++i)
		FileStream.write((char*)&Lod_SkinsFileDataIds[i], 4);
}

void M2Lib::M2Chunk::AFIDChunk::Load(std::fstream& FileStream, UInt32 Size)
{
	UInt32 offs = 0;
	while (offs < Size)
	{
		AnimFileInfo info;
		FileStream.read((char*)&info.AnimId, 2);
		FileStream.read((char*)&info.SubAnimId, 2);
		FileStream.read((char*)&info.FileId, 4);

		AnimFileIds.push_back(info);

		offs += sizeof(AnimFileInfo);
	}
}

void M2Lib::M2Chunk::AFIDChunk::Save(std::fstream& FileStream)
{
	for (auto& info : AnimFileIds)
	{
		FileStream.write((char*)&info.AnimId, 2);
		FileStream.write((char*)&info.SubAnimId, 2);
		FileStream.write((char*)&info.FileId, 4);
	}
}

void M2Lib::M2Chunk::BFIDChunk::Load(std::fstream& FileStream, UInt32 Size)
{
	UInt32 offs = 0;
	while (offs < Size)
	{
		UInt32 boneFileDataId;
		FileStream.read((char*)&boneFileDataId, 4);
		BoneFileDataIds.push_back(boneFileDataId);

		offs += sizeof(UInt32);
	}
}

void M2Lib::M2Chunk::BFIDChunk::Save(std::fstream& FileStream)
{
	for (auto& boneFileDataId : BoneFileDataIds)
		FileStream.write((char*)&boneFileDataId, 4);
}

void M2Lib::M2Chunk::SKIDChunk::Load(std::fstream& FileStream, UInt32 Size)
{
	FileStream.read((char*)&SkeletonFileDataId, 4);
}

void M2Lib::M2Chunk::SKIDChunk::Save(std::fstream& FileStream)
{
	FileStream.write((char*)&SkeletonFileDataId, 4);
}

void M2Lib::M2Chunk::MD21Chunk::Save(std::fstream& FileStream)
{
	assert(false && "Not implemented");
}
