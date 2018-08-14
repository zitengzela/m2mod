#include "ChunkBase.h"
#include <fstream>

void M2Lib::RawChunk::Load(std::fstream& FileStream, UInt32 Size)
{
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), Size);
}

void M2Lib::RawChunk::Save(std::fstream& FileStream)
{
	FileStream.write((char*)RawData.data(), RawData.size());
}