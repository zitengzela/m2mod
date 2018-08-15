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
			Skeleton = 'SKID',
			Texture = 'TXID',
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
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<UInt32> SkinsFileDataIds;
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

		class SKIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			UInt32 SkeletonFileDataId;
		};

		class TXIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<UInt32> TextureFileDataIds;
		};

		class MD21Chunk : public RawChunk
		{
		public:
			void Save(std::fstream& FileStream) override;
		};
	}
}