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
#include "StringUtils.h"
#include <clocale>
#include <algorithm>
#include <array>
#include <iostream>
#include <cmath>
#include <limits.h>
#include "SimdChecker.h"
#include "WzKey.h"

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/fcntl.h>
# include <sys/mman.h>
# include <unistd.h>
#endif

//KMS EMS
byte WzReader::factork[] = {0x4D, 0x23, 0xC7, 0x2B};
//GMS
byte WzReader::factorg[] = {0xB9, 0x7D, 0x63, 0xE9};
/*DES KEY*/
byte WzReader::key[] =
        {
                0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
                0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
                0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00};

namespace WzKey {
    extern uint8_t emsWzKey[65536];
};

WzReader::WzReader(const std::string& p) : length(0), pos(0), file(nullptr), path(p) {

#ifdef USE_MMAP
	#ifdef _WIN32
		fileHandle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
			OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, nullptr);
		if (fileHandle == INVALID_HANDLE_VALUE) {
			printf("fail to open this file : %s", p.c_str());
			exit(0);
		}
		map = CreateFileMapping(fileHandle, 0, PAGE_READONLY, 0, 0, nullptr);
		if (!map) {
			printf("fail to open this file : %s", p.c_str());
			exit(0);
		}
		length = GetFileSize(fileHandle, NULL);
		base = static_cast<int8_t*>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0));

	#else
		fileHandle = open(path.c_str(), O_RDONLY);
		if(fileHandle == -1){
			printf("fail to open this file : %s", p.c_str());
			exit(0);
		}
		struct stat finfo;
		if (fstat(fileHandle, &finfo) == -1){
			printf("fail to open this file : %s", p.c_str());
			exit(0);
		}

		length = finfo.st_size;
		base = static_cast<int8_t*>(mmap(nullptr, length, PROT_READ, MAP_SHARED, fileHandle , 0));
	#endif // _WIN32
		off = base;
#else
    file = fopen(path.c_str(), "rb");
    if (file == nullptr) {
        printf("fail to open this file : %s", p.c_str());
        exit(0);
    }
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
#endif
    
}

WzReader::~WzReader() {
#ifdef USE_MMAP
#ifdef _WIN32
	::UnmapViewOfFile(base);
	::CloseHandle(map);
	::CloseHandle(fileHandle);
#else
	munmap(static_cast<void*>(base), length);
	close(fileHandle);
#endif // _WIN32
#else
    if (file != nullptr)
        fclose(file);
#endif

}

auto WzReader::readNullTerminatedString() -> std::string {
    char c;
    std::string str;
    while ((c = read<char>()) != 0) {
        str += c;
    }
    return str;
}

auto WzReader::transitString(int64_t offset) -> std::string {
    uint8_t a = read<uint8_t>();
    switch (a) {
        case 0x00: //这里有�?�很低的概率的情况会发生一个bug，意外�?�取的这�?的节点为0
					// there is a crash bug happend in a low probability,
        case 0x73:
            return readDecryptString();
        case 0x01:
        case 0x1b:
            return readDecryptString(offset + read<int32_t>());
    }
    return "";
}

auto WzReader::readDecryptString() -> std::string {
    auto size = read<int8_t>();
    if (size > 0) {
        //Unicode
        auto slen = size == 127 ? read<uint32_t>() : size;
        if(slen >= USHRT_MAX){
            return "";
        }
        auto b = readBytes(slen * 2);
        std::string s = decryptUnicodeString(b.get(), slen * 2);
        return s;
    }

    if (size < 0) {
        //ASCII
        auto slen = size == -128 ? read<uint32_t>() : -size;
        if(slen >= USHRT_MAX){
            return "";
        }
        auto b = readBytes(slen);
        std::string s = decryptAsciiString(b.get(), slen);
        return s;
    }
    return "";
}

auto WzReader::decryptAsciiString(const int8_t * original, int32_t len) -> std::string {
    std::string s;

    if(len >= s.max_size()){
        return "";
    }
    //如果在这里读取越界 ，那么len非常大，部分平台new的实现不一致，会引发闪退
    //基本上出现的地图闪退都出现在这个;
    char ns[0x10000] = {0};

    if(isSimdAvailable)
    {
#ifdef __SSE__
        auto m1 = reinterpret_cast<__m128i *>(ns);
	    auto m2 = reinterpret_cast<const __m128i *>(original);
	    auto m3 = reinterpret_cast<__m128i *>(WzKey::emsWzNormalKey);


	    for (int i = 0; i <= len >> 4; ++i) {
		    _mm_storeu_si128(m1 + i, _mm_xor_si128(_mm_loadu_si128(m2 + i), _mm_loadu_si128(m3 + i)));
	    }

	    ns[len] = 0;
	    return std::string(ns, len);
#elif defined(__ARM_NEON__)
        auto m1 = reinterpret_cast<__m128i *>(ns);
        auto m2 = reinterpret_cast<const __m128i *>(original);
        auto m3 = reinterpret_cast<__m128i *>(WzKey::emsWzNormalKey);
        
#  if defined(__arm64__) || defined(__aarch64__) // NEON64
        
        for (int i = 0; i <= len >> 4; ++i) {
            vst1q_s64((int64_t *)(m1 + i), veorq_s64(
                    vreinterpretq_m128i_s64(vld1q_s64((const int64_t *)(m2 + i)))
                    ,
                    vreinterpretq_m128i_s64(vld1q_s64((const int64_t *)(m3 + i)))
            ));
        }
#else //NEON
        
        for (int i = 0; i <= len >> 4; ++i) {
            vst1q_s32((int32_t *)(m1 + i), veorq_s32(
                    vreinterpretq_m128i_s32(vld1q_s32((const int32_t *)(m2 + i)))
                    ,
                    vreinterpretq_m128i_s32(vld1q_s32((const int32_t *)(m3 + i)))
            ));
        }
#endif
        ns[len] = 0;
        return std::string(ns, len);
#else
        throw std::runtime_error("Unsupported SIMD architecture");

#endif
    }


    uint8_t factor = 0xAA;

    for (int index = 0; index < len; ++index) {
        uint8_t c = static_cast<uint8_t>(original[index] ^ WzKey::emsWzKey[index] ^ factor++);
        if (c < 0x20 || c >= 0x80) {
            return "";
        }
        s += c;
    }
    return s;
}

auto WzReader::decryptUnicodeString(const int8_t * original, int32_t len) -> std::string {
    std::u16string s;
    if(len / 2 >= s.max_size()){
        return "";
    }
	std::string oout;
	if (isSimdAvailable) 
	{
        char16_t ws[0x8000] = { 0 };
#ifdef __SSE__
        auto m1 = reinterpret_cast<__m128i *>(ws);
		auto m2 = reinterpret_cast<const __m128i *>(original);
		auto m3 = reinterpret_cast<__m128i *>(WzKey::emsWzWideKey);

		for (int i = 0; i <= len >> 3; ++i) {
			_mm_storeu_si128(m1 + i, _mm_xor_si128(_mm_loadu_si128(m2 + i), _mm_loadu_si128(m3 + i)));
		}
#elif defined(__ARM_NEON__)
        auto m1 = reinterpret_cast<__m128i *>(ws);
        auto m2 = reinterpret_cast<const __m128i *>(original);
        auto m3 = reinterpret_cast<__m128i *>(WzKey::emsWzWideKey);

#if defined(__arm64__) || defined(__aarch64__) // NEON64
        for (int i = 0; i <= len >> 3; ++i) {
               //vst1q_s32((int32_t *)m1 + i, veorq_s32(m2[i], m3[i]));

               vst1q_s64((int64_t *)(m1 + i), veorq_s64(
                       vreinterpretq_m128i_s64(vld1q_s64((const int64_t *)(m2 + i)))
                       ,
                       vreinterpretq_m128i_s64(vld1q_s64((const int64_t *)(m3 + i)))
               ));
        }

#else
        
        for (int i = 0; i <= len >> 3; ++i) {
                vst1q_s32((int32_t *)(m1 + i), veorq_s32(
                        vreinterpretq_m128i_s32(vld1q_s32((const int32_t *)(m2 + i)))
                        ,
                        vreinterpretq_m128i_s32(vld1q_s32((const int32_t *)(m3 + i)))
                ));
        }

#endif

#else
        throw std::runtime_error("Unsupported SIMD architecture");
#endif // _WIN32

         s.append(ws, ws + len / 2);
	}
	else 
	{
		const char16_t * originalChar = reinterpret_cast<const char16_t *>(original);
		const int16_t * wideKey = reinterpret_cast<const int16_t *>(WzKey::emsWzKey);
		uint16_t factor = 0xAAAA;
		for (int index = 0; index < len; index = index + 2) {
			char16_t c2 = originalChar[index >> 1] ^ wideKey[index >> 1] ^ factor++;
			s.append(1, c2);
		}
	}
	StringUtils::UTF16ToUTF8(s, oout);
    return oout;
}

auto WzReader::convertString(std::u16string const &ws) -> std::string {
    std::string oout;
    StringUtils::UTF16ToUTF8(ws, oout);
    return oout;
}


auto WzReader::valid() -> bool {
    if (getLength() >= 64) {
        int singnature = read<int32_t>();
        uint64_t datasize = read<uint64_t>();
        int headersize = read<int32_t>();
        std::string copyright = readNullTerminatedString();
        unsigned short versionhash = read<uint16_t>();
        //0x31474b50 PKG1
        if (0x31474b50 == singnature && getLength() == datasize + headersize) {
            for (int version = 0, n = 0; n < 10; ++version) {
                unsigned int factor = 0;
                char s[10] = "";
                sprintf(s, "%d", version);
                for (int i = 0; s[i] != '\0'; i++) {
                    factor = ((factor * 0x20) + s[i]) + 1;
                }
                int num = (factor >> 0x18) & 0xff;
                int num2 = (factor >> 0x10) & 0xff;
                int num3 = (factor >> 0x8) & 0xff;
                int num4 = factor & 0xff;
                byte num5 = (byte) ~((((num ^ num2)) ^ num3) ^ num4);
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

auto WzReader::getVersion() -> int {
    return header.versions[header.conclusion];
}

auto WzReader::computeOffset() -> int {
    unsigned int value = read<uint32_t>();
    unsigned int offset = (unsigned int) (getPosition() - header.size - 4) ^0xffffffff;
    if (header.conclusion > -1) {
        offset = computeOffset(offset, header.factors[header.conclusion], value);
    } else {
        for (int index = 0; index < 10; ++index) {
            unsigned int position = computeOffset(offset, header.factors[index], value);

            if (header.eod == position) {
                offset = position;
                header.conclusion = index;
                break;
            }
        }
    }
    return (int) offset;
}

auto WzReader::computeOffset(unsigned int offset, unsigned int factor, unsigned int value) -> int {
    offset = offset * factor - 0x581c3f6d;
    factor = offset & 0x1f;
    return (((offset << (int) factor) | (offset >> (0x20 - (int) factor))) ^ value) + 0x78;
}

auto WzReader::readDecryptString(int64_t offset) -> std::string {
    uint64_t currentPos = getPosition();
    setPosition(offset);
    std::string result = readDecryptString();
    setPosition(currentPos);
    return result;
}

auto WzReader::readBin(int32_t size) -> void * {
    void *a = off;
    off += size;
    pos += size;
    return a;
}

auto WzReader::readBytes(int32_t length) -> std::unique_ptr<int8_t[]> {
    std::unique_ptr<int8_t[]> array(new int8_t[length]);
	memset(array.get(), 0, length);
#ifdef USE_MMAP
    memcpy(array.get(),off,length);
    off += sizeof(int8_t) * length;
#else
    fread(array.get(), sizeof(int8_t) * length, 1, file);
#endif
    pos += sizeof(int8_t) * length;
    return array;
}

auto WzReader::getOffset(int32_t offset) -> const int8_t*
{
	setPosition(offset);
	return off;
}

auto WzReader::upackInt() -> int32_t {
    int8_t value = read<int8_t>();
    if (-128 == value)
        return read<int32_t>();
    return value;
}

auto WzReader::readByte() -> int8_t {
    return read<int8_t>();
}

auto WzReader::readShort() -> int16_t {
    return read<int16_t>();
}

auto WzReader::readUShort() -> uint16_t {
    return read<uint16_t>();
}

auto WzReader::readInt() -> int32_t {
    return read<int32_t>();
}

auto WzReader::readDouble() -> double {
    return read<double>();
}

auto WzReader::upackFloat() -> float {
    int8_t value = read<int8_t>();
    if (-128 == value)
        return read<float>();
    return value;
}

auto WzReader::upackLong() -> int64_t {
    int8_t value = read<int8_t>();

    if (-128 == value)
        return read<int64_t>();
    return value;
}

auto WzReader::setPosition(int64_t position) -> bool {
  
    if(position >= length){
       printf("OutOfFileSizeException");
       return false;
    }
#ifndef USE_MMAP
    fseek(file, position, SEEK_SET);
#else
    off = base + position;
#endif
    this->pos = position;
    return true;
}

auto WzReader::decryptBytes(int8_t bytes[], int32_t len) -> int8_t * {
    for (int index = 0; index < len; ++index)
        bytes[index] = (byte) (bytes[index] ^ WzKey::emsWzKey[index]);
    return bytes;
}

auto WzReader::setRegion(char c) -> void {
    //cryptoc = 'k' == c ? cryptok : 'g' == c ? cryptog : cryptox;
}
