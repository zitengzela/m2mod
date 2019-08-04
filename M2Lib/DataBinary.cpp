#include "DataBinary.h"
#include <cassert>

void M2Lib::DataBinary::_SwitchEndianness(char* Data, uint32_t Size)
{
	if (Size <= 1)
		return;

	std::reverse(Data, Data + Size);
}

void M2Lib::DataBinary::_Read(char* Data, uint32_t Size)
{
	_Stream->read(Data, Size);
	if (_Endianness != _EndiannessNative)
		_SwitchEndianness(Data, Size);
}

void M2Lib::DataBinary::_Write(char* Data, uint32_t Size)
{
	if (_Endianness != _EndiannessNative)
		_SwitchEndianness(Data, Size);
	
	_Stream->write(Data, Size);
}

M2Lib::DataBinary::DataBinary(std::fstream* Stream, EEndianness Endianness)
{
	uint8_t EndianTest[2] = { 1, 0 };
	_EndiannessNative = ((*(int16_t*)EndianTest == 1) ? EEndianness_Little : EEndianness_Big);

	assert(Stream);
	_Stream = Stream;
	_Endianness = Endianness;
}

void M2Lib::DataBinary::SwitchEndiannessIfNeeded(char* Data, uint32_t Size) const
{
	if (_Endianness != _EndiannessNative)
		_SwitchEndianness(Data, Size);
}

std::fstream* M2Lib::DataBinary::GetStream() const
{
	return _Stream;
}

void M2Lib::DataBinary::SetStream(std::fstream* Stream)
{
	assert(Stream);
	_Stream = Stream;
}

M2Lib::EEndianness M2Lib::DataBinary::GetEndianness() const
{
	return _Endianness;
}

void M2Lib::DataBinary::SetEndianness(EEndianness Endianness)
{
	_Endianness = Endianness;
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
		char value = Read<char>();
		if (!value)
			break;

		string += value;
	}

	return string;
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
		Write<char>(c);

	Write<char>('\0');
}

M2Lib::C2Vector M2Lib::DataBinary::ReadC2Vector()
{
	C2Vector out;
	out.X = Read<float>();
	out.Y = Read<float>();
	return out;
}

M2Lib::C3Vector M2Lib::DataBinary::ReadC3Vector()
{
	C3Vector out;
	out.X = Read<float>();
	out.Y = Read<float>();
	out.Z = Read<float>();
	return out;
}

void M2Lib::DataBinary::WriteC2Vector(C2Vector const& Vector)
{
	Write<float>(Vector.X);
	Write<float>(Vector.Y);
}

void M2Lib::DataBinary::WriteC3Vector(C3Vector const& Vector)
{
	Write<float>(Vector.X);
	Write<float>(Vector.Y);
	Write<float>(Vector.Z);
}
