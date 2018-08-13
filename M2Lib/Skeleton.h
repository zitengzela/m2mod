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
		}


	public:

	public:
		// loads an M2 skin from a file.
		EError Load(const Char16* FileName);
		// saves this M2 skin to a file.
		EError Save(const Char16* FileName);

		ChunkBase* GetChunk(SkeletonChunk::ESkeletonChunk ChunkId);

	private:
		std::map<SkeletonChunk::ESkeletonChunk, ChunkBase*> Chunks;
	};
}
