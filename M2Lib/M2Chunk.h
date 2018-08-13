#pragma once

#include "ChunkBase.h"

namespace M2Lib
{
	namespace M2Chunk
	{
		enum class EM2Chunk : UInt32
		{
			Model = 'MD21',
			Physic = 'PFID',
			Animation = 'AFID',
			Skin = 'SFID',
			Bone = 'BFID',
		};

		class PFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			UInt32 PhysFileId;
		};

		class SFIDChunk : public ChunkBase
		{
		public:
			SFIDChunk(UInt32 SkinCount);

			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<UInt32> SkinsFileDataIds;
			std::vector<UInt32> Lod_SkinsFileDataIds;

			bool HasLodSkins() const { return Lod_SkinsFileDataIds.size() > 0; }
		};

		class AFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			struct AnimFileInfo
			{
				UInt16 AnimId;
				UInt16 SubAnimId;
				UInt32 FileId;
			};

			std::vector<AnimFileInfo> AnimFileIds;
		};

		class BFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<UInt32> BoneFileDataIds;
		};

		class MD21Chunk : public RawChunk
		{
		public:
			void Save(std::fstream& FileStream) override;
		};
	}
}