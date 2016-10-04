#pragma once

#include "BaseTypes.h"
#include "M2Types.h"

namespace M2Lib
{
	namespace M2Chunk
	{
		// named chunks
		enum EChunk
		{
			EChunk_Model = 0,
			EChunk_Physic,
			EChunk_Animation,
			EChunk_Skin,
			EChunk_Bone,

			EChunk__Count__
		};

		const static UInt32 kChunkIDs[EChunk__Count__] = { 'MD21', 'PFID', 'AFID', 'SFID', 'BFID' };


		class ChunkBase
		{
		public:
			virtual void Load(std::fstream& FileStream, UInt32 Size) = 0;
			virtual void Save(std::fstream& FileStream) = 0;
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
			SFIDChunk(UInt32 SkinCount, UInt32 LodCount);

			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<UInt32> SkinsFileDataIds;
			std::vector<UInt32> Lod_SkinsFileDataIds;
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

		class RawChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<UInt8> RawData;
		};

		class MD21Chunk : public RawChunk
		{
		public:
			void Save(std::fstream& FileStream) override;
		};
	}
}