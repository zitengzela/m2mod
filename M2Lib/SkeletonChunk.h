#pragma once

#include "ChunkBase.h"
#include "DataElement.h"

namespace M2Lib
{
	namespace SkeletonChunk
	{
		enum class ESkeletonChunk : uint32_t
		{
			SKL1 = 'SKL1',
			SKA1 = 'SKA1',
			SKB1 = 'SKB1',
			SKS1 = 'SKS1',
			SKPD = 'SKPD',
			AFID = 'AFID',
			BFID = 'BFID',
		};

#pragma pack(push,1)
		class SKL1Chunk : public ChunkBase
		{
			struct CHeader
			{
				uint32_t _0x00;
				uint32_t nName;
				uint32_t oName;
				uint8_t _0x0c[4];
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(uint32_t DataSize);

		public:
			enum EElement
			{
				EElement_Name = 0,
				EElement_Count
			};

			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKA1Chunk : public ChunkBase
		{
			struct CHeader
			{
				uint32_t nAttachment;
				uint32_t oAttachment;
				uint32_t nAttachmentLookup;
				uint32_t oAttachmentLookup;
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(uint32_t DataSize);

		public:
			enum EElement
			{
				EElement_Attachment = 0,
				EElement_AttachmentLookup,
				EElement_Count
			};

			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKB1Chunk : public ChunkBase
		{
			struct CHeader
			{
				uint32_t nBone;
				uint32_t oBone;
				uint32_t nKeyBoneLookup;
				uint32_t oKeyBoneLookup;
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(uint32_t DataSize);

		public:
			enum EElement
			{
				EElement_Bone = 0,
				EElement_KeyBoneLookup,
				EElement_Count
			};

			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKS1Chunk : public ChunkBase
		{
			struct CHeader
			{
				uint32_t nGlobalSequence;		// 4
				uint32_t oGlobalSequence;

				uint32_t nAnimation;			// 64
				uint32_t oAnimation;

				uint32_t nAnimationLookup;	// 2
				uint32_t oAnimationLookup;
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(uint32_t DataSize);

		public:
			enum EElement
			{
				EElement_GlobalSequence = 0,
				EElement_Animation,
				EElement_AnimationLookup,
				EElement_Count
			};

			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKPDChunk : public ChunkBase
		{
		public:
			struct TData
			{
				uint8_t _0x00[8];
				uint32_t ParentSkeletonFileId;
				uint8_t _0x0c[4];
			} Data;

			ASSERT_SIZE(TData, 16);

			void Load(std::fstream& FileStream, uint32_t Size) override;
			void Save(std::fstream& FileStream) override;
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
#pragma pack(pop)
	}
}