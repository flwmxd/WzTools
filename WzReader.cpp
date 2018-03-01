//////////////////////////////////////////////////////////////////////////////
// This file is part of the PharaohStroy MMORPG client                      // 
// Copyright ?2016-2017 Prime Zeng                                          // 
//                                                                          // 
// This program is free software: you can redistribute it and/or modify     // 
// it under the terms of the GNU Affero General Public License as           // 
// published by the Free Software Foundation, either version 3 of the       // 
// License, or (at your option) any later version.                          // 
//                                                                          // 
// This program is distributed in the hope that it will be useful,          // 
// but WITHOUT ANY WARRANTY; without even the implied warranty of           // 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            // 
// GNU Affero General Public License for more details.                      // 
//                                                                          // 
// You should have received a copy of the GNU Affero General Public License // 
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    // 
////////////////////////////////////////////////////////////////////////////// 

#include "WzReader.h"
#include "../Others/Console.h"
#include "WzKey.h"
#include <locale.h>
#include <algorithm>
#include <array>
#include <iostream>
#include "../Others/StringUtils.h"
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

//KMS EMS 
byte WzReader::factork[] = { 0x4D, 0x23, 0xC7, 0x2B };
//GMS
byte WzReader::factorg[] = { 0xB9, 0x7D, 0x63, 0xE9 };
/*DES KEY*/
byte WzReader::key[] =
{
	0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
};

WzReader::WzReader() :length(0), file(nullptr), pos(0)
{
}


WzReader::WzReader(const std::string & p):path(p) , length(0), file(nullptr), pos(0)
{


	file = fopen(path.c_str(), "rb");
	if (file == nullptr) {
		Log("fail to open this file : %s", p.c_str());
		exit(0);
	}
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	
/*

	HANDLE file = CreateFileA(p.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	if (file == INVALID_HANDLE_VALUE)
	{
		Log("error file ");
		return;
	}

	HANDLE map = CreateFileMappingA(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!map)
	{
		Log("can`t create mapping ");
		return;
	}

//	int32_t offset = 64 * 1024;
	auto size= GetFileSize(file, nullptr);
	base = reinterpret_cast<char *>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));
	if (!base)
	{
		auto a = GetLastError();
		throw;
		Log("can`t map view");
		return;
	}

	off = base;


*/



}



WzReader::~WzReader()
{
	if(file != nullptr)
	fclose(file);
}

auto WzReader::readNullTerminatedString() -> std::string
{
	char c;
	std::string str;
	while ((c = read<char>()) != 0)
	{
		str += c;
	}
	return str;
}

auto WzReader::transitString(int64_t offset) -> std::string
{
	auto a = readByte();
	switch (a)
	{
	case 0x00:
	case 0x73: return readDecryptString();
	case 0x01:
	case 0x1b: return readDecryptString(offset + read<int32_t>());
//	default: throw std::runtime_error("Unknown property string type: " + std::to_string(a));
	}
	return "";
}

auto WzReader::readDecryptString() -> std::string
{
	auto size = read<int8_t>();
	if (size > 0) {
		//Unicode
		auto slen = size == 127 ? read<uint32_t>() : size;
		byte * b = readBytes(slen * 2);
		std::string s = decryptUnicodeString(b, slen * 2);
		delete[]b;
		return s;
	}

	if (size<0) {
		//ASCII
		auto slen = size == -128 ? read<uint32_t>() : -size;
		byte * b = readBytes(slen);
		std::string s = decryptAsciiString(b, slen);
		delete[]b;
		return s;
	}
	

	return "";
}


auto WzReader::decryptAsciiString(byte * bytes, int len) -> std::string
{

	std::string s;
	s.resize(len);
	byte factor = 0xaa;//170
	for (int index = 0; index < len; ++index, ++factor) {
		s [index] = (char)(bytes[index] ^ WzKey::emsWzKey[index] ^ factor);
	}
	return s;
}

auto WzReader::decryptUnicodeString(byte * bytes, int len) -> std::string
{

	std::u16string s;
	int factor = 0xaaaa;
	for (int index = 0; index < len; index = index + 2, ++factor) {
		char16_t c = ((char16_t)(((bytes[index + 1] ^ WzKey::emsWzKey[index + 1] ^ (factor >> 0x08)) << 0x08) + (bytes[index] ^ WzKey::emsWzKey[index] ^ (factor & 0xff))));
		s.append(1,c);
	}
	std::string oout;
	StringUtils::UTF16ToUTF8(s, oout);
	return oout;
}

auto WzReader::convertString(std::u16string const & ws) -> std::string{
	std::string oout;
	StringUtils::UTF16ToUTF8(ws, oout);
	return oout;
}

auto WzReader::valid() -> bool
{
	if (getLength() >= 64) {
		int singnature = read<int32_t>();
		uint64_t datasize = read<uint64_t>();
		int headersize = read<int32_t>();
		std::string copyright = readNullTerminatedString();
		unsigned short versionhash = read<uint16_t>();
		//0x31474b50 PKG1
		if (0x31474b50 == singnature && getLength() == datasize + headersize)
		{
			for (int version = 0, n = 0; n < 10; ++version)
			{
				unsigned int factor = 0;
				char s[10] = "";
	     		sprintf(s, "%d", version);
				for (int i = 0; s[i] != '\0'; i++)
				{
					factor = ((factor * 0x20) + s[i]) + 1;
				}
				int num = (factor >> 0x18) & 0xff;
				int num2 = (factor >> 0x10) & 0xff;
				int num3 = (factor >> 0x8) & 0xff;
				int num4 = factor & 0xff;
				byte num5 = (byte)~((((num^num2)) ^ num3) ^ num4);
				if (num5 == versionhash) {
					header.versions[n] = version;
					header.factors[n] = factor;
					n++;
				}
			}
			header.size = headersize;
			return true;
		}
	}
	return false;
}

auto WzReader::getVersion() -> int
{
	return header.versions[header.conclusion];
}

auto WzReader::computeOffset() -> int
{
	unsigned int value = read<uint32_t>();
	unsigned int offset = (unsigned int)(getPosition() - header.size - 4) ^ 0xffffffff;
	if (header.conclusion > -1) {
		offset = computeOffset(offset, header.factors[header.conclusion], value);
	}
	else {
		for (int index = 0; index < 10; ++index)
		{
			unsigned int position = computeOffset(offset, header.factors[index], value);

			if (header.eod == position)
			{
				offset = position;
				header.conclusion = index;
				break;
			}
		}
	}
	return (int)offset;
}

auto WzReader::computeOffset(unsigned int offset, unsigned int factor, unsigned int value) -> int
{
	offset = offset * factor - 0x581c3f6d;
	factor = offset & 0x1f;
	return (((offset << (int)factor) | (offset >> (0x20 - (int)factor))) ^ value) + 0x78;
}

auto WzReader::readDecryptString(int64_t offset) -> std::string
{
	uint64_t currentPos = getPosition();
	setPosition(offset);
	std::string result = readDecryptString();
	setPosition(currentPos);
	return result;
}

auto WzReader::readBin(int32_t size) -> void *
{
	void *a = off;
	off += size;
	pos += size;
	return a;
}


auto WzReader::readBytes(int length) -> byte *
{

	byte * array = new byte[length];
	fread(array, sizeof(byte)*length, 1, file);
	pos += sizeof(byte)*length;
	return array;

	//return (byte*)readBin(length);
}

auto WzReader::upackInt() -> int32_t
{
	int8_t value = read<int8_t>();
	if (-128 == value)
		return read<int32_t>();
	return value;
}


auto WzReader::readByte() ->byte
{
	return read<byte>();
}

auto WzReader::readShort() -> int16_t
{
	return read<int16_t>();
}


auto WzReader::readUShort() -> uint16_t
{
	return read<uint16_t>();
}

auto WzReader::readInt() ->int32_t
{
	return read<int32_t>();
}

auto WzReader::readDouble() ->double
{
	return read<double>();

}

auto WzReader::upackFloat() -> float
{
	int8_t value = read<int8_t>();
	if (-128 == value)
		return read<float>();
	return value;
}

auto WzReader::upackLong() -> int64_t
{
	int8_t value = read<int8_t>();

	if (-128 == value)
		return read<int64_t>();
	return value;
}

auto WzReader::getFilePath() -> std::string&
{
	return path;
}

auto WzReader::getLength() -> int64_t const
{
	return length;
}

auto WzReader::setPosition(int64_t position) -> void
{
	this->pos = position;
	//off = base + position;

	if (fseek(file, (long)position, SEEK_SET) == -1) {
		//throw std::out_of_range("OutOfFileSizeException");
	}

	
}

auto WzReader::getPosition() -> int64_t const
{
	return pos;
}


auto WzReader::decryptBytes(byte bytes[], int len) -> byte *
{
	for (int index = 0; index < len; ++index)
		bytes[index] = (byte)(bytes[index] ^ WzKey::emsWzKey[index]);
	return bytes;
}

auto WzReader::setRegion(char c) -> void
{
	//cryptoc = 'k' == c ? cryptok : 'g' == c ? cryptog : cryptox;
}
