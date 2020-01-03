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
#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include "WzTools.h"


#define USE_MMAP


class WzReader
{
	struct WzHeader
	{
		int32_t eod; // end of directory
		int32_t size = 0;
		int32_t versions[50] = {0};
		uint32_t factors[50] = {0};
		int32_t conclusion = -1;
	};

public:
	WzReader() = default;
	WzReader(const std::string& path);
	WzReader(const WzReader& reader) = delete;
	WzReader& operator=(const WzReader& reader) = delete;
	~WzReader();
	auto readNullTerminatedString() -> std::string;
	auto transitString(int64_t offset) -> std::string;
	auto readDecryptString() -> std::string;
	auto readDecryptString(int64_t offset) -> std::string;
	auto readBin(int32_t size) -> void*;

	template <typename T>
	inline auto read() -> T
	{
#ifdef USE_MMAP
		auto & v = *reinterpret_cast<T const *>(off);
        off += sizeof(T);
#else
		T v;
		fread(&v, sizeof(T), 1, file);
#endif
		pos += sizeof(T);
		return v;
	}

	auto readByte() -> int8_t;
	auto readShort() -> int16_t;
	auto readUShort() -> uint16_t;
	auto readInt() -> int32_t;
	auto readDouble() -> double;
	auto readBytes(int32_t length) -> std::unique_ptr<int8_t[]>;
	auto getOffset(int32_t length) -> const int8_t*;
	auto upackInt() -> int32_t;
	auto upackFloat() -> float;
	auto upackLong() -> int64_t;

	auto setPosition(int64_t position) -> bool;
	/*ASCII*/
	auto decryptAsciiString(std::unique_ptr<int8_t[]> & bytes, int32_t len) -> std::string;
	/*Unicode*/
	auto decryptUnicodeString(std::unique_ptr<int8_t[]> & bytes, int32_t len) -> std::string;

	auto decryptBytes(int8_t bytes[], int32_t len) -> int8_t*;
	auto setRegion(char c) -> void;
	auto convertString(std::u16string const& p_str) -> std::string;

	inline auto getFile() -> FILE* { return file; };
	inline auto& getFilePath() const { return path; };
	inline auto getLength()  const { return length; };
	inline auto getPosition() const { return pos; };


public:
	//header methods
	auto valid() -> bool; //
	auto getVersion() -> int;
	auto computeOffset() -> int;
	WzHeader header;

private:
	auto computeOffset(unsigned int offset, unsigned int factor, unsigned int value) -> int;

private:
	int64_t length = 0;
	uint64_t pos = 0;
	FILE* file = nullptr;
	std::string path;

#ifdef USE_MMAP
	int8_t * base;
	int8_t * off;
#ifdef _WIN32
	void* fileHandle = nullptr;
	void* map = nullptr;
#else
	int32_t fileHandle = 0;
#endif
	
#endif

private:
	//KMS
	byte cryptok[0xffff];
	//GMS
	byte cryptog[0xffff];
	//others
	byte cryptox[0xffff];
	//CMS
	byte cryptoc[0xffff];

public:
	static byte factorg[];
	static byte factork[];
	static byte key[];
};
