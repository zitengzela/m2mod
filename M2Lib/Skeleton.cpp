#include "Skeleton.h"
#include "DataBinary.h"
#include "Logger.h"
#include "FileSystem.h"
#include "StringHelpers.h"
#include <filesystem>

using namespace M2Lib;
using namespace M2Lib::SkeletonChunk;

EError Skeleton::Load(const wchar_t* FileName)
{
	// check path
	if (!FileName)
		return EError_FailedToLoadSkeleton_NoFileSpecified;
	
	sLogger.LogInfo(L"Loading skeleton at %s", FileName);

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToLoadSkeleton_CouldNotOpenFile;

	// find file size
	FileStream.seekg(0, std::ios::end);
	uint32_t FileSize = (uint32_t)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	sLogger.LogInfo(L"Loading skeleton chunks...");
	while (FileStream.tellg() < FileSize)
	{
		uint32_t ChunkId;
		uint32_t ChunkSize;

		FileStream.read((char*)&ChunkId, sizeof(ChunkId));
		FileStream.read((char*)&ChunkSize, sizeof(ChunkSize));

		ChunkBase* Chunk = NULL;
		auto eChunk = (SkeletonChunk::ESkeletonChunk)REVERSE_CC(ChunkId);
		switch (eChunk)
		{
			case ESkeletonChunk::SKL1: Chunk = new SKL1Chunk(); break;
			case ESkeletonChunk::SKA1: Chunk = new SKA1Chunk(); break;
			case ESkeletonChunk::SKB1: Chunk = new SKB1Chunk(); break;
			case ESkeletonChunk::SKS1: Chunk = new SKS1Chunk(); break;
			case ESkeletonChunk::SKPD: Chunk = new SKPDChunk(); break;
			case ESkeletonChunk::AFID: Chunk = new AFIDChunk(); break;
			case ESkeletonChunk::BFID: Chunk = new BFIDChunk(); break;
			default:
				Chunk = new RawChunk();
				break;
		}

		sLogger.LogInfo(L"Loaded %s skeleton chunk, size %u", ChunkIdToStr(ChunkId, false).c_str(), ChunkSize);

		uint32_t savePos = (uint32_t)FileStream.tellg();
		Chunk->Load(FileStream, ChunkSize);
		FileStream.seekg(savePos + ChunkSize, std::ios::beg);

		Chunks[eChunk] = Chunk;
	}
	sLogger.LogInfo(L"Finished loading skeleton chunks");

	return EError_OK;
}

EError Skeleton::Save(const wchar_t* FileName)
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

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToSaveM2;

	sLogger.LogInfo(L"Saving skeleton to %s", FileName);

	// SKS1 chunk must be loaded before other animation-dependent chunks (checked client)
	std::list<ESkeletonChunk> ExplicitOrder = { ESkeletonChunk::SKL1, ESkeletonChunk::SKS1 };
	for (auto chunk : Chunks)
	{
		if (std::find(ExplicitOrder.begin(), ExplicitOrder.end(), chunk.first) != ExplicitOrder.end())
			continue;

		ExplicitOrder.push_back(chunk.first);
	}

	for (auto chunkId : ExplicitOrder)
	{
		auto chunk = GetChunk(chunkId);
		if (!chunk)
			continue;

		uint32_t ChunkId = REVERSE_CC((uint32_t)chunkId);

		FileStream.write((char*)&ChunkId, 4);
		FileStream.seekp(4, std::ios::cur);		// reserve space for chunk size
		uint32_t savePos = (uint32_t)FileStream.tellp();
		chunk->Save(FileStream);
		uint32_t ChunkSize = (uint32_t)FileStream.tellp() - savePos;
		FileStream.seekp(savePos - 4, std::ios::beg);
		FileStream.write((char*)&ChunkSize, 4);
		FileStream.seekp(0, std::ios::end);
	}

	return EError::EError_OK;
}

ChunkBase* Skeleton::GetChunk(ESkeletonChunk ChunkId)
{
	auto chunkItr = Chunks.find(ChunkId);
	if (chunkItr == Chunks.end())
		return NULL;

	return chunkItr->second;
}
