#include "Skeleton.h"
#include "DataBinary.h"

using namespace M2Lib::SkeletonChunk;

M2Lib::EError M2Lib::Skeleton::Load(const Char16* FileName)
{
	// check path
	if (!FileName)
		return EError_FailedToLoadM2_NoFileSpecified;
	
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToLoadM2_CouldNotOpenFile;

	// find file size
	FileStream.seekg(0, std::ios::end);
	UInt32 FileSize = (UInt32)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	while (FileStream.tellg() < FileSize)
	{
		UInt32 ChunkId;
		UInt32 ChunkSize;

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

		UInt32 savePos = FileStream.tellg();
		Chunk->Load(FileStream, ChunkSize);
		FileStream.seekg(savePos + ChunkSize, std::ios::beg);

		Chunks[eChunk] = Chunk;
	}

	return EError::EError_OK;
}

M2Lib::ChunkBase* M2Lib::Skeleton::GetChunk(ESkeletonChunk ChunkId)
{
	auto chunkItr = Chunks.find(ChunkId);
	if (chunkItr == Chunks.end())
		return NULL;

	return chunkItr->second;
}
