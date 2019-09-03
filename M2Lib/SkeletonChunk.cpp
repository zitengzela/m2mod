#include "SkeletonChunk.h"
#include <fstream>
#include <assert.h>
#include <algorithm>

void M2Lib::SkeletonChunk::SKL1Chunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t pos = (uint32_t)FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKL1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKL1Chunk::Save(std::fstream& FileStream)
{
	FileStream.write(RawData.data(), RawData.size());
}

bool M2Lib::SkeletonChunk::SKL1Chunk::IntializeElements(uint32_t DataSize)
{
	Elements[EElement_Name].Count = Header.nName;
	Elements[EElement_Name].Offset = Header.oName;

	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		uint32_t NextOffset = DataSize;
		for (uint32_t j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		m2lib_assert(NextOffset >= Element.Offset && "SKL1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((uint8_t const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKA1Chunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t pos = (uint32_t)FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKA1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKA1Chunk::Save(std::fstream& FileStream)
{
	uint32_t CurrentOffset = sizeof(Header);
	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		if (!Elements[i].Data.empty())
		{
			Elements[i].Offset = CurrentOffset;
			Elements[i].SizeOriginal = Elements[i].Data.size();
			Elements[i].OffsetOriginal = CurrentOffset;

			CurrentOffset += Elements[i].Data.size();
		}
	}

	Header.nAttachment = Elements[EElement_Attachment].Count;
	Header.oAttachment = Elements[EElement_Attachment].Offset;

	Header.nAttachmentLookup = Elements[EElement_AttachmentLookup].Count;
	Header.oAttachmentLookup = Elements[EElement_AttachmentLookup].Offset;

	uint32_t StartPos = (uint32_t)FileStream.tellp();
	FileStream.write((char*)&Header, sizeof(Header));
	// save elements
	for (uint32_t i = 0; i != EElement_Count; i++)
		m2lib_assert("Failed to write chunk element" && Elements[i].Save(FileStream, StartPos));
}

bool M2Lib::SkeletonChunk::SKA1Chunk::IntializeElements(uint32_t DataSize)
{
	Elements[EElement_Attachment].Count = Header.nAttachment;
	Elements[EElement_Attachment].Offset = Header.oAttachment;

	Elements[EElement_AttachmentLookup].Count = Header.nAttachmentLookup;
	Elements[EElement_AttachmentLookup].Offset = Header.oAttachmentLookup;

	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		uint32_t NextOffset = DataSize;
		for (uint32_t j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		m2lib_assert(NextOffset >= Element.Offset && "SKA1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((uint8_t const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKB1Chunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t pos = (uint32_t)FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKB1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKB1Chunk::Save(std::fstream& FileStream)
{
	uint32_t CurrentOffset = sizeof(Header);
	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		if (!Elements[i].Data.empty())
		{
			Elements[i].Offset = CurrentOffset;
			Elements[i].SizeOriginal = Elements[i].Data.size();
			Elements[i].OffsetOriginal = CurrentOffset;

			CurrentOffset += Elements[i].Data.size();
		}
	}

	Header.nBone = Elements[EElement_Bone].Count;
	Header.oBone = Elements[EElement_Bone].Offset;

	Header.nKeyBoneLookup = Elements[EElement_KeyBoneLookup].Count;
	Header.oKeyBoneLookup = Elements[EElement_KeyBoneLookup].Offset;

	uint32_t StartPos = (uint32_t)FileStream.tellp();
	FileStream.write((char*)&Header, sizeof(Header));
	// save elements
	for (uint32_t i = 0; i != EElement_Count; i++)
		m2lib_assert("Failed to write chunk element" && Elements[i].Save(FileStream, StartPos));
}

bool M2Lib::SkeletonChunk::SKB1Chunk::IntializeElements(uint32_t DataSize)
{
	Elements[EElement_Bone].Count = Header.nBone;
	Elements[EElement_Bone].Offset = Header.oBone;

	Elements[EElement_KeyBoneLookup].Count = Header.nKeyBoneLookup;
	Elements[EElement_KeyBoneLookup].Offset = Header.oKeyBoneLookup;

	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		uint32_t NextOffset = DataSize;
		for (uint32_t j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		m2lib_assert(NextOffset >= Element.Offset && "SKB1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((uint8_t const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKS1Chunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t pos = (uint32_t)FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKS1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKS1Chunk::Save(std::fstream& FileStream)
{
	FileStream.write(RawData.data(), RawData.size());
}

bool M2Lib::SkeletonChunk::SKS1Chunk::IntializeElements(uint32_t DataSize)
{
	Elements[EElement_GlobalSequence].Count = Header.nGlobalSequence;
	Elements[EElement_GlobalSequence].Offset = Header.oGlobalSequence;

	Elements[EElement_Animation].Count = Header.nAnimation;
	Elements[EElement_Animation].Offset = Header.oAnimation;

	Elements[EElement_AnimationLookup].Count = Header.nAnimationLookup;
	Elements[EElement_AnimationLookup].Offset = Header.oAnimationLookup;

	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		uint32_t NextOffset = DataSize;
		for (uint32_t j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		m2lib_assert(NextOffset >= Element.Offset && "SKS1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (uint32_t i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((uint8_t const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKPDChunk::Load(std::fstream & FileStream, uint32_t Size)
{
	FileStream.read((char*)&Data, sizeof(Data));
}

void M2Lib::SkeletonChunk::SKPDChunk::Save(std::fstream & FileStream)
{
	FileStream.write((char*)&Data, sizeof(Data));
}

void M2Lib::SkeletonChunk::AFIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t offs = 0;
	while (offs < Size)
	{
		AnimFileInfo info;
		FileStream.read((char*)&info.AnimId, 2);
		FileStream.read((char*)&info.SubAnimId, 2);
		FileStream.read((char*)&info.FileId, 4);

		AnimInfos.push_back(info);

		offs += sizeof(AnimFileInfo);
	}
}

void M2Lib::SkeletonChunk::AFIDChunk::Save(std::fstream& FileStream)
{
	for (auto& info : AnimInfos)
	{
		FileStream.write((char*)&info.AnimId, 2);
		FileStream.write((char*)&info.SubAnimId, 2);
		FileStream.write((char*)&info.FileId, 4);
	}
}

void M2Lib::SkeletonChunk::BFIDChunk::Load(std::fstream& FileStream, uint32_t Size)
{
	uint32_t offs = 0;
	while (offs < Size)
	{
		uint32_t boneFileDataId;
		FileStream.read((char*)&boneFileDataId, 4);
		BoneFileDataIds.push_back(boneFileDataId);

		offs += sizeof(uint32_t);
	}
}

void M2Lib::SkeletonChunk::BFIDChunk::Save(std::fstream& FileStream)
{
	for (auto& boneFileDataId : BoneFileDataIds)
		FileStream.write((char*)&boneFileDataId, 4);
}
