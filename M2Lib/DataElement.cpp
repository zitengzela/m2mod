#include "DataElement.h"
#include <assert.h>
#include <string.h>
#include <algorithm>

M2Lib::DataElement::DataElement()
	: Count(0)
	, Offset(0)
	, Align(16)
{
}

void* M2Lib::DataElement::GetLocalPointer(uint32_t GlobalOffset)
{
	m2lib_assert(GlobalOffset >= Offset);
	GlobalOffset -= Offset;
	m2lib_assert(GlobalOffset < (uint32_t)Data.size());
	return &Data[GlobalOffset];
}

bool M2Lib::DataElement::Load(std::fstream& FileStream, int32_t FileOffset)
{
	if (Data.empty())
		return true;

	FileStream.seekg(Offset + FileOffset, std::ios::beg);
	FileStream.read((char*)Data.data(), Data.size());

	return true;
}

bool M2Lib::DataElement::Load(uint8_t const* RawData, int32_t FileOffset)
{
	if (Data.empty())
		return true;

	memcpy(Data.data(), RawData + Offset + FileOffset, Data.size());
	return true;
}

bool M2Lib::DataElement::Save(std::fstream& FileStream, int32_t FileOffset)
{
	if (Data.empty())
		return true;

	FileStream.seekp(Offset + FileOffset);
	FileStream.write((char*)Data.data(), Data.size());

	return true;
}
void M2Lib::DataElement::Clear()
{
	Data.clear();
	Count = 0;
}

void M2Lib::DataElement::SetDataSize(uint32_t NewCount, uint32_t NewDataSize, bool CopyOldData)
{
	if (Align != 0)
	{
		uint32_t Mod = NewDataSize % Align;
		if (Mod)
			NewDataSize += Align - Mod;
	}

	std::vector<uint8_t> NewData(NewDataSize, 0);
	if (CopyOldData && !Data.empty())
		memcpy(NewData.data(), Data.data(), Data.size() > NewDataSize ? NewDataSize : Data.size());

	Data = NewData;
	Count = NewCount;
}

void M2Lib::DataElement::Clone(DataElement* Source, DataElement* Destination)
{
	Destination->SetDataSize(Source->Count, Source->Data.size(), false);
	memcpy(Destination->Data.data(), Source->Data.data(), Source->Data.size());
}
