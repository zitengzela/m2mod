#pragma once

#include "M2Types.h"
#include <fstream>

namespace M2Lib
{
	enum EEndianness
	{
		EEndianness_Little,
		EEndianness_Big,
		EEndianness_Native,
	};

	class DataBinary
	{
	private:
		std::fstream* _Stream;
		EEndianness _Endianness;
		EEndianness _EndiannessNative;

		static void _SwitchEndianness(char* Data, uint32_t Size);
		void _Read(char* Data, uint32_t Size);
		void _Write(char* Data, uint32_t Size);

	public:
		DataBinary(std::fstream* Stream, EEndianness Endianness);
		~DataBinary() = default;

		void SwitchEndiannessIfNeeded(char* Data, uint32_t Size) const;

		std::fstream* GetStream() const;
		void SetStream(std::fstream* Stream);

		EEndianness GetEndianness() const;
		void SetEndianness(EEndianness Endianness);

		uint32_t ReadFourCC();
		std::string ReadASCIIString();
		C2Vector ReadC2Vector();
		C3Vector ReadC3Vector();

		void WriteFourCC(uint32_t Value);
		void WriteASCIIString(std::string const& value);
		void WriteC2Vector(C2Vector const& Vector);
		void WriteC3Vector(C3Vector const& Vector);

		template <class T>
		T Read()
		{
			T Result;
			_Read(reinterpret_cast<char*>(&Result), sizeof(Result));
			return Result;
		}

		template <class T>
		void Write(T const& Value)
		{
			T copy = Value;
			_Write(reinterpret_cast<char*>(&copy), sizeof(Value));
		}
	};
}
