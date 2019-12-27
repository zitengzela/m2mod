#pragma once

#include "ChunkBase.h"

namespace M2Lib
{
	namespace M2Chunk
	{
		enum class EM2Chunk : uint32_t
		{
			Model = 'MD21',
			Physic = 'PFID',
			Animation = 'AFID',
			Skin = 'SFID',
			Bone = 'BFID',
			Skeleton = 'SKID',
			Texture = 'TXID',
			TXAC = 'TXAC',
			ParticleGeometryModels = 'GPID',
			ParticleRecursiveModel = 'RPID'

		};

		class PFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			uint32_t PhysFileId;
		};

		class SFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<uint32_t> SkinsFileDataIds;
		};

		class AFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			struct AnimFileInfo
			{
				uint16_t AnimId;
				uint16_t SubAnimId;
				uint32_t FileId;
			};

			std::vector<AnimFileInfo> AnimInfos;
		};

		class BFIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<uint32_t> BoneFileDataIds;
		};

		class SKIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			uint32_t SkeletonFileDataId;
		};

		class TXIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<uint32_t> TextureFileDataIds;
		};

		class TXACChunk : public ChunkBase
		{
		public:
			struct texture_ac
			{
				char unk[2]; // likely used in CM2SceneRender::SetupTextureTransforms and uploaded to the shader directly. 0 otherwise.
			};

			TXACChunk(uint32_t TextureFlagsCount, uint32_t ParticleEmitterCount);

			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<texture_ac> TextureFlagsAC;
			std::vector<texture_ac> ParticleEmitterAC;
		};

		class GPIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<uint32_t> FileDataIds;
		};

		class RPIDChunk : public ChunkBase
		{
		public:
			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			std::vector<uint32_t> FileDataIds;
		};

		class MD21Chunk : public RawChunk
		{
		public:
			void Save(std::fstream& FileStream) override;
		};
	}
}