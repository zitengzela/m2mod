#pragma once

#include "BaseTypes.h"
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

		static void _SwitchEndianness(void* Data, uint8_t Size);
		void _Read(void* Data, uint32_t Size);
		void _Write(void* Data, uint32_t Size);

	public:
		DataBinary(std::fstream* Stream, EEndianness Endianness);
		~DataBinary();

		void SwitchEndiannessIfNeeded(void* Data, uint8_t Size);

		std::fstream* GetStream();
		void SetStream(std::fstream* Stream);

		EEndianness GetEndianness();
		void SetEndianness(EEndianness Endianness);

		uint32_t ReadUInt32();
		int32_t ReadSInt32();
		uint16_t ReadUInt16();
		int16_t ReadSInt16();
		uint8_t ReadUInt8();
		int8_t ReadSInt8();
		float ReadFloat32();
		wchar_t ReadWideChar();
		char ReadChar();

		uint32_t ReadFourCC();
		std::string ReadASCIIString();
		C2Vector ReadC2Vector();
		C3Vector ReadC3Vector();

		void WriteUInt32(uint32_t Value);
		void WriteSInt32(int32_t Value);
		void WriteUInt16(uint16_t Value);
		void WriteSInt16(int16_t Value);
		void WriteUInt8(uint8_t Value);
		void WriteSInt8(int8_t Value);
		void WriteFloat32(float Value);
		void WriteWideChar(wchar_t Value);
		void WriteChar(char Value);
		void WriteFourCC(uint32_t Value);
		void WriteASCIIString(std::string const& value);
		void WriteC2Vector(C2Vector const& Vector);
		void WriteC3Vector(C3Vector const& Vector);
	};
}
