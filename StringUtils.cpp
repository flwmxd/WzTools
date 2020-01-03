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

#include "StringUtils.h"
#include <ctime>
#include <string>
#include <sstream>
#include <memory.h>


//In MapleStrory ,some string like String.wz was stored as UTF-16 in order to internationalization(i18n)
//I use a convert library from cocos2d-x (thanks for it) to convert U16 to U8 ,
//if we use c++11 standard codecvt to convert it to UTF-8 we may occur some crash.(the reason remains to be identified.)
//if the reason identified ,I will replace it to codecvt.
#ifdef SUPPORT_CODECVT
#	include <codecvt>
#endif


/*
* Index into the table below with the first byte of a UTF-8 sequence to
* get the number of trailing bytes that are supposed to follow it.
* Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
* left as-is for anyone who may want to do such conversion, which was
* allowed in earlier algorithms.
*/
static const char trailingBytesForUTF8[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};
static const UTF8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
static const UTF32 offsetsFromUTF8[6] = {
	0x00000000UL, 0x00003080UL, 0x000E2080UL,
	0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

static void trimUTF16VectorFromIndex(std::vector<char16_t>& str, int index)
{
	int size = static_cast<int>(str.size());
	if (index >= size || index < 0)
		return;

	str.erase(str.begin() + index, str.begin() + size);
}

auto StringUtils::replace(std::u16string& src, const std::u16string& origin, const std::u16string& des) -> void
{
	std::u16string::size_type pos = 0;
	std::u16string::size_type srcLen = origin.size();
	std::u16string::size_type desLen = des.size();
	pos = src.find(origin, pos);
	while ((pos != std::u16string::npos))
	{
		src.replace(pos, srcLen, des);
		pos = src.find(origin, (pos + desLen));
	}
}

auto StringUtils::extendId(int32_t id, size_t length) -> std::string
{
	std::string strid = std::to_string(id);
	if (strid.size() < length)
	{
		strid.insert(0, length - strid.size(), '0');
	}
	return strid;
}

auto StringUtils::splitNumber(std::string& input) -> void
{
	for (size_t i = input.size(); i > 3; i -= 3)
	{
		input.insert(i - 3, 1, ',');
	}
}

auto StringUtils::splitNumber(const std::string &input) ->std::string
{
	std::string str = input;
	for (size_t i = input.size(); i > 3; i -= 3)
	{
		str.insert(i - 3, 1, ',');
	}
	return str;
}

auto StringUtils::split(std::string input, const std::string& delimiter) -> std::vector<std::string>
{
	std::vector<std::string> ret;
	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos)
	{
		token = input.substr(0, pos);
		ret.push_back(token);
		input.erase(0, pos + delimiter.length());
	}
	ret.push_back(input);
	return ret;
}

auto StringUtils::startWith(const std::string& str, const std::string& start) -> bool
{
	return str.compare(0, start.size(), start) == 0;
}

auto StringUtils::split(std::string input, const std::string& delimiter, std::vector<std::string>& outs) -> void
{
	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos)
	{
		token = input.substr(0, pos);
		outs.push_back(token);
		input.erase(0, pos + delimiter.length());
	}
	outs.push_back(input);
}

auto StringUtils::split(std::u16string input, const std::u16string& delimiter,
                        std::vector<std::u16string>& outs) -> void
{
	size_t pos = 0;
	std::u16string token;
	while ((pos = input.find(delimiter)) != std::u16string::npos)
	{
		token = input.substr(0, pos);
		outs.push_back(token);
		input.erase(0, pos + delimiter.length());
	}
	outs.push_back(input);
}

auto StringUtils::UTF8ToUTF16(IN const std::string& utf8, OUT std::u16string& outUtf16) -> bool
{
#ifdef SUPPORT_CODECVT
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cvt;
	outUtf16.clear();
	outUtf16.append(cvt.from_bytes(utf8));
	return true;
#else
	if (utf8.empty())
	{
		outUtf16.clear();
		return true;
	}

	bool ret = false;

	const size_t utf16Bytes = (utf8.length() + 1) * sizeof(char16_t);
	char16_t* utf16 = new char16_t[utf16Bytes];
	memset(utf16, 0, utf16Bytes);

	char* utf16ptr = reinterpret_cast<char *>(utf16);
	const UTF8* error = nullptr;

	if (llvm::ConvertUTF8toWide(2, utf8, utf16ptr, error))
	{
		outUtf16 = utf16;
		ret = true;
	}

	delete[] utf16;

	return ret;
#endif
}

auto StringUtils::UTF8ToUTF16(IN const std::string& utf8) -> std::u16string
{
#ifdef SUPPORT_CODECVT
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> cvt;
	return cvt.from_bytes(utf8);
#else
	std::u16string outUtf16;
	if (utf8.empty())
	{
		outUtf16.clear();
		return outUtf16;
	}
	bool ret = false;
	const size_t utf16Bytes = (utf8.length() + 1) * sizeof(char16_t);
	char16_t* utf16 = new char16_t[utf16Bytes];
	memset(utf16, 0, utf16Bytes);
	char* utf16ptr = reinterpret_cast<char *>(utf16);
	const UTF8* error = nullptr;
	if (llvm::ConvertUTF8toWide(2, utf8, utf16ptr, error))
	{
		outUtf16 = utf16;
		ret = true;
	}
#ifndef MS_RELEASE
	if (!ret)
	{
		printf("convert error,call func : %s,line : %d , error : %s ", __func__, __LINE__, error);
	}
#	endif
	delete[] utf16;
	return outUtf16;
#endif
}

auto StringUtils::UTF16ToUTF8(IN const std::u16string& utf16, OUT std::string& outUtf8) -> bool
{
#ifdef SUPPORT_CODECVT
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
	outUtf8.clear();
	outUtf8.append(cvt.to_bytes(utf16));
	return true;
#else
	if (utf16.empty())
	{
		outUtf8.clear();
		return true;
	}
	return llvm::convertUTF16ToUTF8String(utf16, outUtf8);
#endif
}

auto StringUtils::UTF16ToUTF8(const std::u16string& utf16) -> std::string
{
#ifdef SUPPORT_CODECVT
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
	return cvt.to_bytes(utf16);
#else

	std::string outUtf8;
	if (utf16.empty())
	{
		outUtf8.clear();
		return outUtf8;
	}

	bool b = llvm::convertUTF16ToUTF8String(utf16, outUtf8);
#ifndef MS_RELEASE
	if (!b)
	{
		printf("convert error,call func : %s,line : %d", __func__, __LINE__);
	}
#endif
	return outUtf8;
#endif
}

auto StringUtils::trimUTF16Vector(std::vector<char16_t>& str) -> void
{
	int len = static_cast<int>(str.size());

	if (len <= 0)
		return;

	int last_index = len - 1;

	// Only start trimming if the last character is whitespace..
	if (isUnicodeSpace(str[last_index]))
	{
		for (int i = last_index - 1; i >= 0; --i)
		{
			if (isUnicodeSpace(str[i]))
				last_index = i;
			else
				break;
		}

		trimUTF16VectorFromIndex(str, last_index);
	}
}

auto StringUtils::isUnicodeSpace(char16_t ch) -> bool
{
	return (ch >= 0x0009 && ch <= 0x000D) || ch == 0x0020 || ch == 0x0085 || ch == 0x00A0 || ch == 0x1680 || (ch >=
		0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029 || ch == 0x202F || ch == 0x205F || ch == 0x3000;
}

auto StringUtils::isCJKUnicode(char16_t ch) -> bool
{
	return (ch >= 0x4E00 && ch <= 0x9FBF) // CJK Unified Ideographs
		|| (ch >= 0x2E80 && ch <= 0x2FDF) // CJK Radicals Supplement & Kangxi Radicals
		|| (ch >= 0x2FF0 && ch <= 0x30FF) // Ideographic Description Characters, CJK Symbols and Punctuation & Japanese
		|| (ch >= 0x3100 && ch <= 0x31BF) // Korean
		|| (ch >= 0xAC00 && ch <= 0xD7AF) // Hangul Syllables
		|| (ch >= 0xF900 && ch <= 0xFAFF) // CJK Compatibility Ideographs
		|| (ch >= 0xFE30 && ch <= 0xFE4F) // CJK Compatibility Forms
		|| (ch >= 0x31C0 && ch <= 0x4DFF); // Other exiensions
}

auto StringUtils::getCharacterCountInUTF8String(const std::string& utf8) -> long
{
	return getUTF8StringLength((const UTF8 *)utf8.c_str());
}

auto StringUtils::getIndexOfLastNotChar16(const std::vector<char16_t>& str, char16_t c) -> uint32_t
{
	int len = static_cast<int>(str.size());

	int i = len - 1;
	for (; i >= 0; --i)
		if (str[i] != c)
			return i;

	return i;
}

auto StringUtils::getChar16VectorFromUTF16String(const std::u16string& utf16) -> std::vector<char16_t>
{
	std::vector<char16_t> ret;
	size_t len = utf16.length();
	ret.reserve(len);
	for (size_t i = 0; i < len; ++i)
	{
		ret.push_back(utf16[i]);
	}
	return ret;
}


auto StringUtils::formatTime(int64_t timestamp) -> std::string
{
	std::stringstream strtime;
	char tAll[255];
	std::time_t tt = timestamp;
	tm* tm = std::localtime(&tt);
	if (tm == nullptr)
	{
		return "";
	}
	std::strftime(tAll, sizeof(tAll), "%Y.%m.%d %H:%M:%S", tm);
	strtime << tAll;
	return strtime.str();
}

auto StringUtils::formatTime(int64_t timestamp, const std::string& format) -> std::string
{
	std::stringstream strtime;
	char tAll[255];
	std::time_t tt = timestamp;
	tm* tm = std::localtime(&tt);
	if (tm == nullptr)
	{
		return "";
	}
	std::strftime(tAll, sizeof(tAll), format.c_str(), tm);
	strtime << tAll;
	return strtime.str();
}

auto StringUtils::toUnixTimestamp(int64_t mapleTime) -> int64_t
{
	constexpr int64_t FT_UT_OFFSET = 116444520000000000L;
	std::time_t currenttime = (mapleTime - FT_UT_OFFSET) / 10000 - 14 * 60 * 60 * 1000; //std::time(0);
	return currenttime;
}

auto StringUtils::split(const std::u16string& str, int32_t len, std::vector<std::u16string>& vec) -> void
{
	if (str.length() > len)
	{
		auto s = str.substr(0, len - 1) + u"\\r";
		vec.emplace_back(s);
		split(str.substr(len), len, vec);
	}
	else
	{
		vec.emplace_back(str + u"\\r");
	}
}

auto StringUtils::splitMobData(const std::string& str) -> std::vector<int32_t>
{
	auto newStr = str;
	std::vector<int32_t> vec;
	while (newStr.size() > 2) {
		std::string val = newStr.substr(0, 3);
		newStr = newStr.substr(3);
		vec.emplace_back(std::stoi(val));
	}
	return vec;
}

auto StringUtils::splitMobData(const std::string& str, std::vector<int32_t>& outVec) -> void
{
	auto newStr = str;
	while (newStr.size() > 2) {
		std::string val = newStr.substr(0, 3);
		newStr = newStr.substr(3);
		outVec.emplace_back(std::stoi(val));
	}
}


auto StringUtils::trim(std::string& str) -> void
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" "));
		str.erase(str.find_last_not_of(" ") + 1);
	}
}

auto StringUtils::replace(std::string& str, const std::string& old, const std::string& newStr) -> void
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = old.size();
	std::string::size_type dstlen = newStr.size();

	while ((pos = str.find(old, pos)) != std::string::npos)
	{
		str.replace(pos, srclen, newStr);
		pos += dstlen;
	}
}

auto StringUtils::replaceUtf16(std::u16string& str, const std::u16string& old, const std::u16string& newStr) -> void
{
	std::u16string::size_type pos = 0;
	std::u16string::size_type srclen = old.size();
	std::u16string::size_type dstlen = newStr.size();

	while ((pos = str.find(old, pos)) != std::u16string::npos)
	{
		str.replace(pos, srclen, newStr);
		pos += dstlen;
	}
}

auto StringUtils::trim(std::u16string& str) -> void
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(u" "));
		str.erase(str.find_last_not_of(u" ") + 1);
		str.erase(0, str.find_first_not_of(u"\u3000"));
		str.erase(str.find_last_not_of(u"\u3000") + 1);
	}
}

inline uint16_t SwapByteOrder_16(uint16_t value)
{
#if defined(_MSC_VER) && !defined(_DEBUG)
	// The DLL version of the runtime lacks these functions (bug!?), but in a
	// release build they're replaced with BSWAP instructions anyway.
	return _byteswap_ushort(value);
#else
	uint16_t Hi = value << 8;
	uint16_t Lo = value >> 8;
	return Hi | Lo;
#endif
}

auto llvm::convertUTF16ToUTF8String(const std::u16string& utf16, std::string& Out) -> bool
{
	//assert(Out.empty());

	// Avoid OOB by returning early on empty input.
	if (utf16.empty())
		return true;

	const UTF16* Src = reinterpret_cast<const UTF16 *>(utf16.data());
	const UTF16* SrcEnd = reinterpret_cast<const UTF16 *>(utf16.data() + utf16.length());

	// Byteswap if necessary.
	std::vector<UTF16> ByteSwapped;
	if (Src[0] == UNI_UTF16_BYTE_ORDER_MARK_SWAPPED)
	{
		ByteSwapped.insert(ByteSwapped.end(), Src, SrcEnd);
		for (size_t I = 0, E = ByteSwapped.size(); I != E; ++I)
			ByteSwapped[I] = SwapByteOrder_16(ByteSwapped[I]);
		Src = &ByteSwapped[0];
		SrcEnd = &ByteSwapped[ByteSwapped.size() - 1] + 1;
	}

	// Skip the BOM for conversion.
	if (Src[0] == UNI_UTF16_BYTE_ORDER_MARK_NATIVE)
		Src++;

	// Just allocate enough space up front.  We'll shrink it later.
	Out.resize(utf16.length() * UNI_MAX_UTF8_BYTES_PER_CODE_POINT + 1);
	UTF8* Dst = reinterpret_cast<UTF8 *>(&Out[0]);
	UTF8* DstEnd = Dst + Out.size();

	ConversionResult CR =
		ConvertUTF16toUTF8(&Src, SrcEnd, &Dst, DstEnd, strictConversion);

	if (CR != conversionOK)
	{
		Out.clear();
		return false;
	}

	Out.resize(reinterpret_cast<char *>(Dst) - &Out[0]);
	return true;
}

auto llvm::ConvertUTF8toWide(unsigned WideCharWidth, const std::string& Source, char*& ResultPtr,
                             const UTF8*& ErrorPtr) -> bool
{
	//	assert(WideCharWidth == 1 || WideCharWidth == 2 || WideCharWidth == 4);
	ConversionResult result = conversionOK;
	// Copy the character span over.
	if (WideCharWidth == 1)
	{
		const UTF8* Pos = reinterpret_cast<const UTF8 *>(Source.data());
		if (!isLegalUTF8String(&Pos, reinterpret_cast<const UTF8 *>(Source.data() + Source.length())))
		{
			result = sourceIllegal;
			ErrorPtr = Pos;
		}
		else
		{
			memcpy(ResultPtr, Source.data(), Source.size());
			ResultPtr += Source.size();
		}
	}
	else if (WideCharWidth == 2)
	{
		const UTF8* sourceStart = (const UTF8 *)Source.data();
		// FIXME: Make the type of the result buffer correct instead of
		// using reinterpret_cast.
		UTF16* targetStart = reinterpret_cast<UTF16 *>(ResultPtr);
		ConversionFlags flags = strictConversion;
		result = ConvertUTF8toUTF16(
			&sourceStart, sourceStart + Source.size(),
			&targetStart, targetStart + 2 * Source.size(), flags);
		if (result == conversionOK)
			ResultPtr = reinterpret_cast<char *>(targetStart);
		else
			ErrorPtr = sourceStart;
	}
	else if (WideCharWidth == 4)
	{
		const UTF8* sourceStart = (const UTF8 *)Source.data();
		// FIXME: Make the type of the result buffer correct instead of
		// using reinterpret_cast.
		UTF32* targetStart = reinterpret_cast<UTF32 *>(ResultPtr);
		ConversionFlags flags = strictConversion;
		result = ConvertUTF8toUTF32(
			&sourceStart, sourceStart + Source.size(),
			&targetStart, targetStart + 4 * Source.size(), flags);
		if (result == conversionOK)
			ResultPtr = reinterpret_cast<char *>(targetStart);
		else
			ErrorPtr = sourceStart;
	}
	return result == conversionOK;
}

auto llvm::ConvertCodePointToUTF8(unsigned Source, char*& ResultPtr) -> bool
{
	const UTF32* SourceStart = &Source;
	const UTF32* SourceEnd = SourceStart + 1;
	UTF8* TargetStart = reinterpret_cast<UTF8 *>(ResultPtr);
	UTF8* TargetEnd = TargetStart + 4;
	ConversionResult CR = ConvertUTF32toUTF8(&SourceStart, SourceEnd,
	                                         &TargetStart, TargetEnd,
	                                         strictConversion);
	if (CR != conversionOK)
		return false;

	ResultPtr = reinterpret_cast<char *>(TargetStart);
	return true;
}

auto ConvertUTF16toUTF8(const UTF16** sourceStart, const UTF16* sourceEnd, UTF8** targetStart, UTF8* targetEnd,
                        ConversionFlags flags) -> ConversionResult
{
	ConversionResult result = conversionOK;
	const UTF16* source = *sourceStart;
	UTF8* target = *targetStart;
	while (source < sourceEnd)
	{
		UTF32 ch;
		unsigned short bytesToWrite = 0;
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80;
		const UTF16* oldSource = source; /* In case we have to back up because of target overflow. */
		ch = *source++;
		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END)
		{
			/* If the 16 bits following the high surrogate are in the source buffer... */
			if (source < sourceEnd)
			{
				UTF32 ch2 = *source;
				/* If it's a low surrogate, convert to UTF32. */
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END)
				{
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
					++source;
				}
				else if (flags == strictConversion)
				{
					/* it's an unpaired high surrogate */
					--source; /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
			}
			else
			{
				/* We don't have the 16 bits following the high surrogate. */
				--source; /* return to the high surrogate */
				result = sourceExhausted;
				break;
			}
		}
		else if (flags == strictConversion)
		{
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END)
			{
				--source; /* return to the illegal value itself */
				result = sourceIllegal;
				break;
			}
		}
		/* Figure out how many bytes the result will require */
		if (ch < (UTF32)0x80)
		{
			bytesToWrite = 1;
		}
		else if (ch < (UTF32)0x800)
		{
			bytesToWrite = 2;
		}
		else if (ch < (UTF32)0x10000)
		{
			bytesToWrite = 3;
		}
		else if (ch < (UTF32)0x110000)
		{
			bytesToWrite = 4;
		}
		else
		{
			bytesToWrite = 3;
			ch = UNI_REPLACEMENT_CHAR;
		}

		target += bytesToWrite;
		if (target > targetEnd)
		{
			source = oldSource; /* Back up source pointer! */
			target -= bytesToWrite;
			result = targetExhausted;
			break;
		}
		switch (bytesToWrite)
		{
			/* note: everything falls through. */
		case 4:
			*--target = (UTF8)((ch | byteMark) & byteMask);
			ch >>= 6;
		case 3:
			*--target = (UTF8)((ch | byteMark) & byteMask);
			ch >>= 6;
		case 2:
			*--target = (UTF8)((ch | byteMark) & byteMask);
			ch >>= 6;
		case 1:
			*--target = (UTF8)(ch | firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
	}
	*sourceStart = source;
	*targetStart = target;
	return result;
}

auto ConvertUTF32toUTF8(const UTF32** sourceStart, const UTF32* sourceEnd, UTF8** targetStart, UTF8* targetEnd,
                        ConversionFlags flags) -> ConversionResult
{
	ConversionResult result = conversionOK;
	const UTF32* source = *sourceStart;
	UTF8* target = *targetStart;
	while (source < sourceEnd)
	{
		UTF32 ch;
		unsigned short bytesToWrite = 0;
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80;
		ch = *source++;
		if (flags == strictConversion)
		{
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)
			{
				--source; /* return to the illegal value itself */
				result = sourceIllegal;
				break;
			}
		}
		/*
         * Figure out how many bytes the result will require. Turn any
         * illegally large UTF32 things (> Plane 17) into replacement chars.
         */
		if (ch < (UTF32)0x80)
		{
			bytesToWrite = 1;
		}
		else if (ch < (UTF32)0x800)
		{
			bytesToWrite = 2;
		}
		else if (ch < (UTF32)0x10000)
		{
			bytesToWrite = 3;
		}
		else if (ch <= UNI_MAX_LEGAL_UTF32)
		{
			bytesToWrite = 4;
		}
		else
		{
			bytesToWrite = 3;
			ch = UNI_REPLACEMENT_CHAR;
			result = sourceIllegal;
		}

		target += bytesToWrite;
		if (target > targetEnd)
		{
			--source; /* Back up source pointer! */
			target -= bytesToWrite;
			result = targetExhausted;
			break;
		}
		switch (bytesToWrite)
		{
			/* note: everything falls through. */
		case 4:
			*--target = (UTF8)((ch | byteMark) & byteMask);
			ch >>= 6;
		case 3:
			*--target = (UTF8)((ch | byteMark) & byteMask);
			ch >>= 6;
		case 2:
			*--target = (UTF8)((ch | byteMark) & byteMask);
			ch >>= 6;
		case 1:
			*--target = (UTF8)(ch | firstByteMark[bytesToWrite]);
		}
		target += bytesToWrite;
	}
	*sourceStart = source;
	*targetStart = target;
	return result;
}

auto ConvertUTF8toUTF16(const UTF8** sourceStart, const UTF8* sourceEnd, UTF16** targetStart, UTF16* targetEnd,
                        ConversionFlags flags) -> ConversionResult
{
	ConversionResult result = conversionOK;
	const UTF8* source = *sourceStart;
	UTF16* target = *targetStart;
	while (source < sourceEnd)
	{
		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
		if (extraBytesToRead >= sourceEnd - source)
		{
			result = sourceExhausted;
			break;
		}
		/* Do this check whether lenient or strict */
		if (!isLegalUTF8(source, extraBytesToRead + 1))
		{
			result = sourceIllegal;
			break;
		}
		/*
		* The cases all fall through. See "Note A" below.
		*/
		switch (extraBytesToRead)
		{
		case 5:
			ch += *source++;
			ch <<= 6; /* remember, illegal UTF-8 */
		case 4:
			ch += *source++;
			ch <<= 6; /* remember, illegal UTF-8 */
		case 3:
			ch += *source++;
			ch <<= 6;
		case 2:
			ch += *source++;
			ch <<= 6;
		case 1:
			ch += *source++;
			ch <<= 6;
		case 0:
			ch += *source++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];

		if (target >= targetEnd)
		{
			source -= (extraBytesToRead + 1); /* Back up source pointer! */
			result = targetExhausted;
			break;
		}
		if (ch <= UNI_MAX_BMP)
		{
			/* Target is a character <= 0xFFFF */
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)
			{
				if (flags == strictConversion)
				{
					source -= (extraBytesToRead + 1); /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
				*target++ = UNI_REPLACEMENT_CHAR;
			}
			else
			{
				*target++ = (UTF16)ch; /* normal case */
			}
		}
		else if (ch > UNI_MAX_UTF16)
		{
			if (flags == strictConversion)
			{
				result = sourceIllegal;
				source -= (extraBytesToRead + 1); /* return to the start */
				break; /* Bail out; shouldn't continue */
			}
			*target++ = UNI_REPLACEMENT_CHAR;
		}
		else
		{
			/* target is a character in range 0xFFFF - 0x10FFFF. */
			if (target + 1 >= targetEnd)
			{
				source -= (extraBytesToRead + 1); /* Back up source pointer! */
				result = targetExhausted;
				break;
			}
			ch -= halfBase;
			*target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
		}
	}
	*sourceStart = source;
	*targetStart = target;
	return result;
}

auto ConvertUTF8toUTF32(const UTF8** sourceStart, const UTF8* sourceEnd, UTF32** targetStart, UTF32* targetEnd,
                        ConversionFlags flags) -> ConversionResult
{
	ConversionResult result = conversionOK;
	const UTF8* source = *sourceStart;
	UTF32* target = *targetStart;
	while (source < sourceEnd)
	{
		UTF32 ch = 0;
		unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
		if (extraBytesToRead >= sourceEnd - source)
		{
			result = sourceExhausted;
			break;
		}
		/* Do this check whether lenient or strict */
		if (!isLegalUTF8(source, extraBytesToRead + 1))
		{
			result = sourceIllegal;
			break;
		}
		/*
		* The cases all fall through. See "Note A" below.
		*/
		switch (extraBytesToRead)
		{
		case 5:
			ch += *source++;
			ch <<= 6;
		case 4:
			ch += *source++;
			ch <<= 6;
		case 3:
			ch += *source++;
			ch <<= 6;
		case 2:
			ch += *source++;
			ch <<= 6;
		case 1:
			ch += *source++;
			ch <<= 6;
		case 0:
			ch += *source++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];

		if (target >= targetEnd)
		{
			source -= (extraBytesToRead + 1); /* Back up the source pointer! */
			result = targetExhausted;
			break;
		}
		if (ch <= UNI_MAX_LEGAL_UTF32)
		{
			/*
			* UTF-16 surrogate values are illegal in UTF-32, and anything
			* over Plane 17 (> 0x10FFFF) is illegal.
			*/
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)
			{
				if (flags == strictConversion)
				{
					source -= (extraBytesToRead + 1); /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
				*target++ = UNI_REPLACEMENT_CHAR;
			}
			else
			{
				*target++ = ch;
			}
		}
		else
		{
			/* i.e., ch > UNI_MAX_LEGAL_UTF32 */
			result = sourceIllegal;
			*target++ = UNI_REPLACEMENT_CHAR;
		}
	}
	*sourceStart = source;
	*targetStart = target;
	return result;
}

auto ConvertUTF16toUTF32(const UTF16** sourceStart, const UTF16* sourceEnd, UTF32** targetStart, UTF32* targetEnd,
                         ConversionFlags flags) -> ConversionResult
{
	ConversionResult result = conversionOK;
	const UTF16* source = *sourceStart;
	UTF32* target = *targetStart;
	UTF32 ch, ch2;
	while (source < sourceEnd)
	{
		const UTF16* oldSource = source; /*  In case we have to back up because of target overflow. */
		ch = *source++;
		/* If we have a surrogate pair, convert to UTF32 first. */
		if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END)
		{
			/* If the 16 bits following the high surrogate are in the source buffer... */
			if (source < sourceEnd)
			{
				ch2 = *source;
				/* If it's a low surrogate, convert to UTF32. */
				if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END)
				{
					ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
					++source;
				}
				else if (flags == strictConversion)
				{
					/* it's an unpaired high surrogate */
					--source; /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
			}
			else
			{
				/* We don't have the 16 bits following the high surrogate. */
				--source; /* return to the high surrogate */
				result = sourceExhausted;
				break;
			}
		}
		else if (flags == strictConversion)
		{
			/* UTF-16 surrogate values are illegal in UTF-32 */
			if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END)
			{
				--source; /* return to the illegal value itself */
				result = sourceIllegal;
				break;
			}
		}
		if (target >= targetEnd)
		{
			source = oldSource; /* Back up source pointer! */
			result = targetExhausted;
			break;
		}
		*target++ = ch;
	}
	*sourceStart = source;
	*targetStart = target;
#ifdef CVTUTF_DEBUG
	if (result == sourceIllegal)
	{
		fprintf(stderr, "ConvertUTF16toUTF32 illegal seq 0x%04x,%04x\n", ch, ch2);
		fflush(stderr);
	}
#endif
	return result;
}

auto ConvertUTF32toUTF16(const UTF32** sourceStart, const UTF32* sourceEnd, UTF16** targetStart, UTF16* targetEnd,
                         ConversionFlags flags) -> ConversionResult
{
	ConversionResult result = conversionOK;
	const UTF32* source = *sourceStart;
	UTF16* target = *targetStart;
	while (source < sourceEnd)
	{
		UTF32 ch;
		if (target >= targetEnd)
		{
			result = targetExhausted;
			break;
		}
		ch = *source++;
		if (ch <= UNI_MAX_BMP)
		{
			/* Target is a character <= 0xFFFF */
			/* UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values */
			if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END)
			{
				if (flags == strictConversion)
				{
					--source; /* return to the illegal value itself */
					result = sourceIllegal;
					break;
				}
				*target++ = UNI_REPLACEMENT_CHAR;
			}
			else
			{
				*target++ = (UTF16)ch; /* normal case */
			}
		}
		else if (ch > UNI_MAX_LEGAL_UTF32)
		{
			if (flags == strictConversion)
			{
				result = sourceIllegal;
			}
			else
			{
				*target++ = UNI_REPLACEMENT_CHAR;
			}
		}
		else
		{
			/* target is a character in range 0xFFFF - 0x10FFFF. */
			if (target + 1 >= targetEnd)
			{
				--source; /* Back up source pointer! */
				result = targetExhausted;
				break;
			}
			ch -= halfBase;
			*target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
			*target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
		}
	}
	*sourceStart = source;
	*targetStart = target;
	return result;
}

auto isLegalUTF8String(const UTF8** source, const UTF8* sourceEnd) -> bool
{
	while (*source != sourceEnd)
	{
		int length = trailingBytesForUTF8[**source] + 1;
		if (length > sourceEnd - *source || !isLegalUTF8(*source, length))
			return false;
		*source += length;
	}
	return true;
}

auto isLegalUTF8Sequence(const UTF8* source, const UTF8* sourceEnd) -> bool
{
	int length = trailingBytesForUTF8[*source] + 1;
	if (length > sourceEnd - source)
	{
		return false;
	}
	return isLegalUTF8(source, length);
}

auto isLegalUTF8(const UTF8* source, int length) -> bool
{
	UTF8 a;
	const UTF8* srcptr = source + length;
	switch (length)
	{
	default:
		return false;
		/* Everything else falls through when "true"... */
	case 4:
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
			return false;
	case 3:
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
			return false;
	case 2:
		if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
			return false;

		switch (*source)
		{
			/* no fall-through in this inner switch */
		case 0xE0:
			if (a < 0xA0)
				return false;
			break;
		case 0xED:
			if (a > 0x9F)
				return false;
			break;
		case 0xF0:
			if (a < 0x90)
				return false;
			break;
		case 0xF4:
			if (a > 0x8F)
				return false;
			break;
		default:
			if (a < 0x80)
				return false;
		}

	case 1:
		if (*source >= 0x80 && *source < 0xC2)
			return false;
	}
	if (*source > 0xF4)
		return false;
	return true;
}

auto getUTF8StringLength(const UTF8* utf8) -> int
{
	const UTF8** source = &utf8;
	const UTF8* sourceEnd = utf8 + strlen((const char *)utf8);
	int ret = 0;
	while (*source != sourceEnd)
	{
		int length = trailingBytesForUTF8[**source] + 1;
		if (length > sourceEnd - *source || !isLegalUTF8(*source, length))
			return 0;
		*source += length;
		++ret;
	}
	return ret;
}
