#pragma once

#include "BaseTypes.h"
#include <fstream>
#include <vector>
#include <assert.h>

namespace M2Lib
{

	class DataElement
	{
	public:
		uint32_t Count;				// number of sub-elements contained in Data. the definition of this structure depends on this element's usage.
		uint32_t Offset;				// offset in bytes from begining of file to where this element's data begins.

		uint32_t OffsetOriginal;		// offset of this element as loaded from the original file.
		uint32_t SizeOriginal;		// size of this element as loaded from the original file.

		std::vector<uint8_t> Data;	// our local copy of data. note that DataSize might be greater than sizeof( DataType ) * Count if there is animation data references or padding at the end.
		int32_t Align;				// byte alignment boundary. M2s pad the ends of elements with zeros data so they align on 16 byte boundaries.

	public:
		DataElement();
		~DataElement();

		// given a global offset, returns a pointer to the data contained in this Element.
		// asserts if GlobalOffset lies outside of this element.
		void* GetLocalPointer(uint32_t GlobalOffset);

		// loads this element's data from a file stream. assumes that Offset and DataSize have already been set.
		bool Load(std::fstream& FileStream, int32_t FileOffset);
		// loads this element's data from memory. assumes that Offset and DataSize have already been set.
		bool Load(uint8_t const* RawData, int32_t FileOffset);
		// saves this element's data to a file stream. assumes that Offset and DataSize have already been set.
		bool Save(std::fstream& FileStream, int32_t FileOffset);

		// reallocates Data, either erasing existing data or preserving it.
		// adds padding to NewDataSize if necessary so that new size aligns with Align.
		void SetDataSize(uint32_t NewCount, uint32_t NewDataSize, bool CopyOldData);
		// clears element
		void Clear();

		template <class T>
		T* as() { return (T*)Data.data(); }

		template <class T>
		std::vector<T> asVector()
		{
			assert("Element data size is less than expected" && sizeof(T) * Count <= Data.size());

			std::vector<T> ret(Count);
			if (!Data.empty())
				memcpy(ret.data(), Data.data(), sizeof(T) * Count);

			return ret;
		}

		template <class T>
		T* at(uint32_t Index)
		{
			assert(__FUNCTION__ " Index too large" && Index < Count);

			return &as<T>()[Index];
		}

		// clones this element from Source to Destination.
		static void Clone(DataElement* Source, DataElement* Destination);
	};
}
