#include "DataBinary.h"
#include <assert.h>

void M2Lib::DataBinary::_SwitchEndianness(void* Data, uint8_t Size)
{
	if (Size <= 1)
	{
		return;
	}

	uint8_t* DataOut = new uint8_t[Size];
	uint8_t* DataIn = (uint8_t*)Data;
	for (int32_t i = 0, out = Size - 1; i < Size; i++, out--)
	{
		DataOut[out] = DataIn[i];
	}
	for (int32_t i = 0; i < Size; i++)
	{
		DataIn[i] = DataOut[i];
	}
	delete[] DataOut;
}

void M2Lib::DataBinary::_Read(void* Data, uint32_t Size)
{
	_Stream->read((char*)Data, Size);
	if (_Endianness != _EndiannessNative)
	{
		_SwitchEndianness(Data, Size);
	}
}

void M2Lib::DataBinary::_Write(void* Data, uint32_t Size)
{
	if (_Endianness != _EndiannessNative)
	{
		_SwitchEndianness(Data, Size);
	}
	_Stream->write((const char*)Data, Size);
}

M2Lib::DataBinary::DataBinary(std::fstream* Stream, EEndianness Endianness)
{
	uint8_t EndianTest[2] = { 1, 0 };
	_EndiannessNative = ((*(int16_t*)EndianTest == 1) ? EEndianness_Little : EEndianness_Big);

	assert(Stream);
	_Stream = Stream;
	_Endianness = Endianness;
}

M2Lib::DataBinary::~DataBinary()
{
}

void M2Lib::DataBinary::SwitchEndiannessIfNeeded(void* Data, uint8_t Size)
{
	if (_Endianness != _EndiannessNative)
	{
		_SwitchEndianness(Data, Size);
	}
}

std::fstream* M2Lib::DataBinary::GetStream()
{
	return _Stream;
}

void M2Lib::DataBinary::SetStream(std::fstream* Stream)
{
	assert(Stream);
	_Stream = Stream;
}

M2Lib::EEndianness M2Lib::DataBinary::GetEndianness()
{
	return _Endianness;
}

void M2Lib::DataBinary::SetEndianness(EEndianness Endianness)
{
	_Endianness = Endianness;
}

uint32_t M2Lib::DataBinary::ReadUInt32()
{
	uint32_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

int32_t M2Lib::DataBinary::ReadSInt32()
{
	int32_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

uint16_t M2Lib::DataBinary::ReadUInt16()
{
	uint16_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

int16_t M2Lib::DataBinary::ReadSInt16()
{
	int16_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

uint8_t M2Lib::DataBinary::ReadUInt8()
{
	uint8_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

int8_t M2Lib::DataBinary::ReadSInt8()
{
	int8_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

float M2Lib::DataBinary::ReadFloat32()
{
	float Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

wchar_t M2Lib::DataBinary::ReadWideChar()
{
	wchar_t Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

char M2Lib::DataBinary::ReadChar()
{
	char Result;
	_Read(&Result, sizeof(Result));
	return Result;
}

uint32_t M2Lib::DataBinary::ReadFourCC()
{
	uint32_t Result = 0;
	char CharA;
	char CharB;
	char CharC;
	char CharD;
	_Read(&CharA, 1);
	_Read(&CharB, 1);
	_Read(&CharC, 1);
	_Read(&CharD, 1);
	return MakeFourCC(CharA, CharB, CharC, CharD);
}

std::string M2Lib::DataBinary::ReadASCIIString()
{
	std::string string;
	for (;;)
	{
		char value = ReadChar();
		if (!value)
			break;

		string += value;
	}

	return string;
}

void M2Lib::DataBinary::WriteUInt32(uint32_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteSInt32(int32_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteUInt16(uint16_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteSInt16(int16_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteUInt8(uint8_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteSInt8(int8_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteFloat32(float Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteWideChar(wchar_t Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteChar(char Value)
{
	_Write(&Value, sizeof(Value));
}

void M2Lib::DataBinary::WriteFourCC(uint32_t Value)
{
	char CharA = (Value);
	char CharB = (Value >> 8);
	char CharC = (Value >> 16);
	char CharD = (Value >> 24);
	_Write(&CharA, 1);
	_Write(&CharB, 1);
	_Write(&CharC, 1);
	_Write(&CharD, 1);
}

void M2Lib::DataBinary::WriteASCIIString(std::string const& value)
{
	for (auto c : value)
		WriteChar(c);

	WriteChar('\0');
}

M2Lib::C2Vector M2Lib::DataBinary::ReadC2Vector()
{
	C2Vector out;
	out.X = ReadFloat32();
	out.Y = ReadFloat32();
	return out;
}

M2Lib::C3Vector M2Lib::DataBinary::ReadC3Vector()
{
	C3Vector out;
	out.X = ReadFloat32();
	out.Y = ReadFloat32();
	out.Z = ReadFloat32();
	return out;
}

void M2Lib::DataBinary::WriteC2Vector(C2Vector const& Vector)
{
	WriteFloat32(Vector.X);
	WriteFloat32(Vector.Y);
}

void M2Lib::DataBinary::WriteC3Vector(C3Vector const& Vector)
{
	WriteFloat32(Vector.X);
	WriteFloat32(Vector.Y);
	WriteFloat32(Vector.Z);
}
