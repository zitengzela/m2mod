#pragma once

#include "ChunkBase.h"
#include "DataElement.h"

namespace M2Lib
{
	namespace SkeletonChunk
	{
		enum class ESkeletonChunk : UInt32
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
				UInt32 _0x00;
				UInt32 nName;
				UInt32 oName;
				UInt8 _0x0c[4];
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(UInt32 DataSize);

		public:
			enum EElement
			{
				EElement_Name = 0,
				EElement_Count
			};

			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKA1Chunk : public ChunkBase
		{
			struct CHeader
			{
				UInt32 nAttachment;
				UInt32 oAttachment;
				UInt32 nAttachmentLookup;
				UInt32 oAttachmentLookup;
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(UInt32 DataSize);

		public:
			enum EElement
			{
				EElement_Attachment = 0,
				EElement_AttachmentLookup,
				EElement_Count
			};

			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKB1Chunk : public ChunkBase
		{
			struct CHeader
			{
				UInt32 nBone;
				UInt32 oBone;
				UInt32 nKeyBoneLookup;
				UInt32 oKeyBoneLookup;
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(UInt32 DataSize);

		public:
			enum EElement
			{
				EElement_Bone = 0,
				EElement_KeyBoneLookup,
				EElement_Count
			};

			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};

		class SKS1Chunk : public ChunkBase
		{
			struct CHeader
			{
				UInt32 nGlobalSequence;		// 4
				UInt32 oGlobalSequence;

				UInt32 nAnimation;			// 64
				UInt32 oAnimation;

				UInt32 nAnimationLookup;	// 2
				UInt32 oAnimationLookup;
			} Header;

			std::vector<char> RawData;

			bool IntializeElements(UInt32 DataSize);

		public:
			enum EElement
			{
				EElement_GlobalSequence = 0,
				EElement_Animation,
				EElement_AnimationLookup,
				EElement_Count
			};

			void Load(std::fstream& FileStream, UInt32 Size) override;
			void Save(std::fstream& FileStream) override;

			DataElement Elements[EElement_Count];
		};
#pragma pack(pop)
	}
}