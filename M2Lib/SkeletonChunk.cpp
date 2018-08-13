#include "SkeletonChunk.h"
#include <fstream>
#include <assert.h>
#include <algorithm>

void M2Lib::SkeletonChunk::SKL1Chunk::Load(std::fstream& FileStream, UInt32 Size)
{
	UInt32 pos = FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKL1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKL1Chunk::Save(std::fstream& FileStream)
{
	assert(false && "Method not implemented");
}

bool M2Lib::SkeletonChunk::SKL1Chunk::IntializeElements(UInt32 DataSize)
{
	Elements[EElement_Name].Count = Header.nName;
	Elements[EElement_Name].Offset = Header.oName;

	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = DataSize;
		for (UInt32 j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		assert(NextOffset >= Element.Offset && "SKL1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((UInt8 const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKA1Chunk::Load(std::fstream& FileStream, UInt32 Size)
{
	UInt32 pos = FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKA1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKA1Chunk::Save(std::fstream& FileStream)
{
	assert(false && "Method not implemented");
}

bool M2Lib::SkeletonChunk::SKA1Chunk::IntializeElements(UInt32 DataSize)
{
	Elements[EElement_Attachment].Count = Header.nAttachment;
	Elements[EElement_Attachment].Offset = Header.oAttachment;

	Elements[EElement_AttachmentLookup].Count = Header.nAttachmentLookup;
	Elements[EElement_AttachmentLookup].Offset = Header.oAttachmentLookup;

	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = DataSize;
		for (UInt32 j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		assert(NextOffset >= Element.Offset && "SKA1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((UInt8 const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKB1Chunk::Load(std::fstream& FileStream, UInt32 Size)
{
	UInt32 pos = FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKB1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKB1Chunk::Save(std::fstream& FileStream)
{
	assert(false && "Method not implemented");
}

bool M2Lib::SkeletonChunk::SKB1Chunk::IntializeElements(UInt32 DataSize)
{
	Elements[EElement_Bone].Count = Header.nBone;
	Elements[EElement_Bone].Offset = Header.oBone;

	Elements[EElement_KeyBoneLookup].Count = Header.nKeyBoneLookup;
	Elements[EElement_KeyBoneLookup].Offset = Header.oKeyBoneLookup;

	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = DataSize;
		for (UInt32 j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		assert(NextOffset >= Element.Offset && "SKB1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((UInt8 const*)RawData.data(), 0))
			return false;
	}

	return true;
}

void M2Lib::SkeletonChunk::SKS1Chunk::Load(std::fstream& FileStream, UInt32 Size)
{
	UInt32 pos = FileStream.tellg();
	FileStream.read((char*)&Header, sizeof(SKS1Chunk::Header));

	FileStream.seekg(pos);
	RawData.resize(Size);
	FileStream.read((char*)RawData.data(), RawData.size());

	IntializeElements(Size);
}

void M2Lib::SkeletonChunk::SKS1Chunk::Save(std::fstream& FileStream)
{
	assert(false && "Method not implemented");
}

bool M2Lib::SkeletonChunk::SKS1Chunk::IntializeElements(UInt32 DataSize)
{
	Elements[EElement_GlobalSequence].Count = Header.nGlobalSequence;
	Elements[EElement_GlobalSequence].Offset = Header.oGlobalSequence;

	Elements[EElement_Animation].Count = Header.nAnimation;
	Elements[EElement_Animation].Offset = Header.oAnimation;

	Elements[EElement_AnimationLookup].Count = Header.nAnimationLookup;
	Elements[EElement_AnimationLookup].Offset = Header.oAnimationLookup;

	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = DataSize;
		for (UInt32 j = 0; j < EElement_Count; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}

		assert(NextOffset >= Element.Offset && "SKS1 elements are in wrong order");
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}

	// load elements
	for (UInt32 i = 0; i < EElement_Count; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load((UInt8 const*)RawData.data(), 0))
			return false;
	}

	return true;
}