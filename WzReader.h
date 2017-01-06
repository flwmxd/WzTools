#pragma once
#include <iostream>
#include <string>
#include <stdio.h>
#ifdef _WIN32

#endif // _WIN32
#include <codecvt>
#include "WzTools.h"
class WzReader
{
	struct WzHeader {
		int eod ;// end of directory
		int size = 0;
		int versions[50] = {0};
		unsigned int factors[50] = { 0 };
		int conclusion = -1;
	};

public:
	WzReader();
	WzReader(std::string path);
	WzReader(const WzReader &reader) = default;
	~WzReader();
	auto readNullTerminatedString() -> std::string;
	auto transitString(int offset) ->std::string;
	auto readDecryptString()->std::string;
	auto readDecryptString(int offset)->std::string;
	template <typename T>
	auto read()->T;
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
	auto valid() -> bool;//��֤
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
	char16_t const * u16key = nullptr;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;

};

