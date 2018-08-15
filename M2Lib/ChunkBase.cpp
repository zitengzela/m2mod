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

std::string M2Lib::ChunkIdToStr(UInt32 ChunkId, bool Reverse)
{
	std::string s((char*)&ChunkId, 4);
	if (Reverse)
		std::reverse(s.begin(), s.end());

	return s;
}
