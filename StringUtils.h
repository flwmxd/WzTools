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
#include <string>
#include <vector>
#include <stdarg.h>
#include <memory>
typedef uint32_t UTF32;   /* at least 32 bits */
typedef uint16_t UTF16; /* at least 16 bits */
typedef uint8_t UTF8;   /* typically 8 bits */

static const int halfShift = 10; /* used for shifting by 10 bits */
static const UTF32 halfBase = 0x0010000UL;
static const UTF32 halfMask = 0x3FFUL;

#define UNI_SUR_HIGH_START (UTF32)0xD800
#define UNI_SUR_HIGH_END (UTF32)0xDBFF
#define UNI_SUR_LOW_START (UTF32)0xDC00
#define UNI_SUR_LOW_END (UTF32)0xDFFF

/* Some fundamental constants */
#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_BMP (UTF32)0x0000FFFF
#define UNI_MAX_UTF16 (UTF32)0x0010FFFF
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (UTF32)0x0010FFFF

#define UNI_MAX_UTF8_BYTES_PER_CODE_POINT 4

#define UNI_UTF16_BYTE_ORDER_MARK_NATIVE 0xFEFF
#define UNI_UTF16_BYTE_ORDER_MARK_SWAPPED 0xFFFE

#define OUT
#define IN
#define Utf16(x) StringUtils::UTF8ToUTF16(x)
#define Utf8(x) StringUtils::UTF16ToUTF8(x)
typedef enum
{
	conversionOK,
	/* conversion successful */
	sourceExhausted,
	/* partial character in source, but hit end */
	targetExhausted,
	/* insuff. room in target for conversion */
	sourceIllegal /* source sequence is illegal/malformed */
} ConversionResult;

typedef enum
{
	strictConversion = 0,
	lenientConversion
} ConversionFlags;

namespace StringUtils
{
template <typename T>
auto or_default(const std::string &str, T def) -> T
{
	try
	{
		int32_t intval = std::stoi(str.c_str());

		return static_cast<T>(intval);
	}
	catch (...)
	{
		return def;
	}
}

template <typename T>
auto or_zero(const std::string &str)  -> T
{
	return or_default<T>(str, T(0));
}

template <typename... Args>
auto format(const std::string &format, Args... args) -> std::string
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args...);
	return std::string(buf.get(), buf.get() + size - 1);
}

auto replace(std::u16string &src, const std::u16string &origin, const std::u16string &des) -> void;
auto extendId(int32_t id, size_t length) -> std::string;
/**
  * 格式化数字
  * format Numbers
  */
auto splitNumber(std::string &input) -> void;
auto splitNumber(const std::string &input) ->std::string;
auto split(std::string input, const std::string &delimiter) -> std::vector<std::string>;
auto split(std::u16string input, const std::u16string &delimiter, std::vector<std::u16string> &outs) -> void;
auto split(std::string input, const std::string &delimiter, std::vector<std::string> &outs) -> void;
auto startWith(const std::string &str, const std::string &start) -> bool;
/**
  *  @brief Converts utf8 string to utf16 string.
  *  @param utf8 The utf8 string to be converted.
  *  @param outUtf16 The output utf16 string.
  *  @return True if succeed, otherwise false.
  *  @note Please check the return value before using \p outUtf16
  *  e.g.
  *  @code
  *    std::u16string utf16;
  *    bool ret = StringUtils::UTF8ToUTF16("你好hello", utf16);
  *    if (ret) {
  *        do_some_thing_with_utf16(utf16);
  *    }
  *  @endcode
  */
auto UTF8ToUTF16(IN const std::string &utf8, OUT std::u16string &outUtf16) -> bool;
auto UTF8ToUTF16(IN const std::string &utf8) -> std::u16string;
/**
  *  @brief Converts utf16 string to utf8 string.
  *  @param utf16 The utf16 string to be converted.
  *  @param outUtf8 The output utf8 string.
  *  @return True if succeed, otherwise false.
  *  @note Please check the return value before using \p outUtf8
  *  e.g.
  *  @code
  *    std::string utf8;
  *    bool ret = StringUtils::UTF16ToUTF8(u"\u4f60\u597d", utf16);
  *    if (ret) {
  *        do_some_thing_with_utf8(utf8);
  *    }
  *  @endcode
  */
auto UTF16ToUTF8(IN const std::u16string &utf16, OUT std::string &outUtf8) -> bool;
auto UTF16ToUTF8(const std::u16string &utf16) -> std::string;

/**
  *  @brief Trims the unicode spaces at the end of char16_t vector.
  */
auto trimUTF16Vector(std::vector<char16_t> &str) -> void;

/**
		*  @brief Whether the character is a whitespace character.
		*  @param ch    The unicode character.
		*  @returns     Whether the character is a white space character.
		*/
auto isUnicodeSpace(char16_t ch) -> bool;
/**
		*  @brief Whether the character is a Chinese/Japanese/Korean character.
		*  @param ch    The unicode character.
		*  @returns     Whether the character is a Chinese character.
		*/
auto isCJKUnicode(char16_t ch) -> bool;
/**
*  @brief Returns the length of the string in characters.
*  @param utf8 An UTF-8 encoded string.
*  @returns The length of the string in characters.
*/
auto getCharacterCountInUTF8String(const std::string &utf8) -> long;
/**
		*  @brief Gets the index of the last character that is not equal to the character given.
		*  @param str   The string to be searched.
		*  @param c     The character to be searched for.
		*  @returns The index of the last character that is not \p c.
		*/
auto getIndexOfLastNotChar16(const std::vector<char16_t> &str, char16_t c) -> uint32_t;
/**
*  @brief Gets char16_t vector from a given utf16 string.
*/
auto getChar16VectorFromUTF16String(const std::u16string &utf16) -> std::vector<char16_t>;


auto split(const std::u16string &str, int32_t len, std::vector<std::u16string> &vec) -> void;
auto splitMobData(const std::string& str)->std::vector<int32_t>;
auto splitMobData(const std::string& str, std::vector<int32_t> & outVec)->void;
auto formatTime(int64_t timestamp) -> std::string;
auto formatTime(int64_t timestamp, const std::string &format) -> std::string;
auto toUnixTimestamp(int64_t mapleTime) -> int64_t;
auto trim(std::string &str) -> void;
auto trim(std::u16string &str) -> void;
auto replace(std::string &str, const std::string &old, const std::string &newStr) -> void;
auto replaceUtf16(std::u16string &str, const std::u16string &old, const std::u16string &newStr) -> void;
}; // namespace StringUtils
namespace llvm
{
auto convertUTF16ToUTF8String(const std::u16string &utf16, std::string &Out) -> bool;
auto ConvertUTF8toWide(unsigned WideCharWidth, const std::string &Source, char *&ResultPtr,
					   const UTF8 *&ErrorPtr) -> bool;
auto ConvertCodePointToUTF8(unsigned Source, char *&ResultPtr) -> bool;
}; // namespace llvm

auto ConvertUTF32toUTF8(const UTF32 **sourceStart, const UTF32 *sourceEnd, UTF8 **targetStart, UTF8 *targetEnd,
						ConversionFlags flags) -> ConversionResult;
auto ConvertUTF8toUTF16(const UTF8 **sourceStart, const UTF8 *sourceEnd, UTF16 **targetStart, UTF16 *targetEnd,
						ConversionFlags flags) -> ConversionResult;
auto ConvertUTF8toUTF32(const UTF8 **sourceStart, const UTF8 *sourceEnd, UTF32 **targetStart, UTF32 *targetEnd,
						ConversionFlags flags) -> ConversionResult;
auto ConvertUTF16toUTF32(const UTF16 **sourceStart, const UTF16 *sourceEnd, UTF32 **targetStart, UTF32 *targetEnd,
						 ConversionFlags flags) -> ConversionResult;
auto ConvertUTF32toUTF16(const UTF32 **sourceStart, const UTF32 *sourceEnd, UTF16 **targetStart, UTF16 *targetEnd,
						 ConversionFlags flags) -> ConversionResult;
auto ConvertUTF16toUTF8(const UTF16 **sourceStart, const UTF16 *sourceEnd, UTF8 **targetStart, UTF8 *targetEnd,
						ConversionFlags flags) -> ConversionResult;
auto isLegalUTF8String(const UTF8 **source, const UTF8 *sourceEnd) -> bool;
auto isLegalUTF8Sequence(const UTF8 *source, const UTF8 *sourceEnd) -> bool;
auto isLegalUTF8(const UTF8 *source, int length) -> bool;
auto getUTF8StringLength(const UTF8 *utf8) -> int;
