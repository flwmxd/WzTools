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
#ifdef _WIN32

#endif // _WIN32
//#include <codecvt>
#include "WzTools.h"
class WzReader
{
	struct WzHeader {
		int32_t eod ;// end of directory
		int32_t size = 0;
		int32_t versions[50] = {0};
		uint32_t factors[50] = { 0 };
		int32_t conclusion = -1;
	};

public:
	WzReader();
	WzReader(const std::string &path);
	WzReader(const WzReader &reader) = delete;
	WzReader &operator=(const WzReader & reader) = delete;
	~WzReader();
	auto readNullTerminatedString() -> std::string;
	auto transitString(int64_t offset) ->std::string;
	auto readDecryptString()->std::string;
	auto readDecryptString(int64_t offset)->std::string;
	auto readBin(int32_t size) -> void*;

	template <typename T>
	auto read()->T 
	{
		T v;
		fread(&v, sizeof(T), 1, file);
		pos += sizeof(T);
		return v;
		//return *(T*)(readBin(sizeof(T)));
	}
	auto readByte()->byte;
	auto readShort() -> int16_t;
	auto readUShort()-> uint16_t;
	auto readInt()->int32_t;
	auto readDouble()->double;
	auto readBytes(int length) ->byte*;
	auto upackInt()->int32_t ;
	auto upackFloat() ->float;
	auto upackLong()->int64_t;

	auto getFilePath() -> std::string&;
	auto getLength()->int64_t const;
	auto setPosition(int64_t position) -> void;
	auto getPosition()->int64_t const;
	/*ASCII*/
	auto decryptAsciiString(byte * bytes, int len) ->std::string;
	/*Unicode*/
	auto decryptUnicodeString(byte * bytes, int len)->std::string;
	auto decryptBytes(byte bytes[], int len)-> byte*;
	auto setRegion(char c)->void;
	auto convertString(std::u16string const & p_str)->std::string;
public:
	//header methods
	auto valid() -> bool;//
	auto getVersion() -> int;
	auto computeOffset() -> int;
	WzHeader header;
private:
	auto computeOffset(unsigned int offset, unsigned int factor, unsigned int value)-> int;

private:
	int64_t length;
	FILE * file;
	uint64_t pos;
	std::string path;

	char * base;
	char * off;

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

