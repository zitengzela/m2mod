#include "ChunkBase.h"
#include <fstream>
#include "StringHelpers.h"

void M2Lib::RawChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), Size);
}

void M2Lib::RawChunk::Save(std::fstream& FileStream)
{
	FileStream.write((char*)RawData.data(), RawData.size());
}

std::wstring M2Lib::ChunkIdToStr(uint32_t ChunkId, bool Reverse)
{
	std::string s((char*)&ChunkId, 4);
	if (Reverse)
		std::reverse(s.begin(), s.end());

	return StringHelpers::StringToWString(s);
}
