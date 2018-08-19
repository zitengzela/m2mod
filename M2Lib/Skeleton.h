#pragma once

#include "DataElement.h"
#include "M2SkinElement.h"
#include "M2Types.h"
#include "SkeletonChunk.h"
#include <vector>
#include <map>

namespace M2Lib
{
	//using namespace M2SkinElement;

	class Skeleton
	{
	public:

#pragma pack(push,1)

#pragma pack(pop)

	public:
		Skeleton()
		{
		}

		~Skeleton()
		{
			for (auto chunk : Chunks)
				delete chunk.second;
		}

		EError Load(const Char16* FileName);
		EError Save(const Char16* FileName);

		ChunkBase* GetChunk(SkeletonChunk::ESkeletonChunk ChunkId);

	private:
		std::map<SkeletonChunk::ESkeletonChunk, ChunkBase*> Chunks;
	};
}
