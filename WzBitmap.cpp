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

#include "WzBitmap.h"
#include <zlib.h>
#include "../Others/Console.h"
#include <vector>
#include "WzKey.h"
#ifdef _WIN32
#pragma comment(lib,"zlib.lib")
#endif
#include <algorithm>

#define RGB_PREMULTIPLY_ALPHA(vr, vg, vb, va) \
    (unsigned)(((unsigned)((unsigned char)(vr) * ((unsigned char)(va) + 1)) >> 8) | \
    ((unsigned)((unsigned char)(vg) * ((unsigned char)(va) + 1) >> 8) << 8) | \
    ((unsigned)((unsigned char)(vb) * ((unsigned char)(va) + 1) >> 8) << 16) | \
    ((unsigned)(unsigned char)(va) << 24))

uint8_t const table4[0x10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
uint8_t const table5[0x20] = { 0x00, 0x08, 0x10, 0x19, 0x21, 0x29, 0x31, 0x3A, 0x42, 0x4A, 0x52,
0x5A, 0x63, 0x6B, 0x73, 0x7B, 0x84, 0x8C, 0x94, 0x9C, 0xA5, 0xAD,
0xB5, 0xBD, 0xC5, 0xCE, 0xD6, 0xDE, 0xE6, 0xEF, 0xF7, 0xFF };
uint8_t const table6[0x40] = {
	0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2D, 0x31, 0x35, 0x39, 0x3D,
	0x41, 0x45, 0x49, 0x4D, 0x51, 0x55, 0x59, 0x5D, 0x61, 0x65, 0x69, 0x6D, 0x71, 0x75, 0x79, 0x7D,
	0x82, 0x86, 0x8A, 0x8E, 0x92, 0x96, 0x9A, 0x9E, 0xA2, 0xA6, 0xAA, 0xAE, 0xB2, 0xB6, 0xBA, 0xBE,
	0xC2, 0xC6, 0xCA, 0xCE, 0xD2, 0xD7, 0xDB, 0xDF, 0xE3, 0xE7, 0xEB, 0xEF, 0xF3, 0xF7, 0xFB, 0xFF 
};
template <int N> void scale(std::vector<uint8_t> const & input, std::vector<uint8_t> & output, int width, int height) {
	auto in = reinterpret_cast<uint32_t const *>(input.data());
	auto out = reinterpret_cast<uint32_t *>(output.data());
	auto w = width / N;
	auto h = height / N;
	for (auto y = 0; y < h; ++y) {
		for (auto x = 0; x < w; ++x) {
			auto p = in[y * w + x];
			for (auto yy = y * N; yy < (y + 1) * N; ++yy) {
				for (auto xx = x * N; xx < (x + 1) * N; ++xx) {
					out[yy * width + xx] = p;
				}
			}
		}
	}
};

WzBitmap::WzBitmap()
{
	height = 0;
	width = 0;
	length = 0;
	offset = 0;
	format = 0;
	format2 = 0;
	reader = nullptr;
}


WzBitmap::WzBitmap(int32_t h, int32_t w, uint32_t len, int32_t o, int32_t f, uint8_t f2, std::shared_ptr<WzReader> r) 
	:height(h), width(w), length(len), offset(o), format(f), format2(f2), reader(r)
{

}

WzBitmap::~WzBitmap()
{

}

auto WzBitmap::id()const ->int32_t
{
	return offset;
}

auto WzBitmap::getWidth() const -> int32_t
{
	return width;
}

auto WzBitmap::getHeight() const -> int32_t
{
	return height;
}

auto WzBitmap::setWidth(int32_t width) -> void
{
	this->width = width;
}



auto WzBitmap::content() const ->byte*
{
	reader->setPosition(offset + 1);
	return reader->readBytes(length);
}


auto WzBitmap::data() ->std::vector<uint8_t>&
{
	if (input.size() > 0) {
		return input;
	}
	std::vector<uint8_t> output;
	auto size = width * height * 4;
	auto biggest = std::max(static_cast<uint32_t>(size), length);

	input.resize(biggest);
	output.resize(biggest);

	const uint8_t * original = reinterpret_cast<uint8_t const *>(content());
	auto decompressed = 0;
	auto decompress = [&] {
		z_stream strm = {};
		strm.next_in = input.data();
		strm.avail_in = length;
		inflateInit(&strm);
		strm.next_out = output.data();
		strm.avail_out = static_cast<unsigned>(output.size());
		auto err = inflate(&strm, Z_FINISH);
		if (err != Z_BUF_ERROR) {
			if (err != Z_DATA_ERROR) { std::cerr << "zlib error of " << std::dec << err << std::endl; }
			return false;
		}
		decompressed = static_cast<int>(strm.total_out);
		inflateEnd(&strm);

		return true;
	};
	auto decrypt = [&] {
		auto p = 0u;
		for (auto i = 0u; i <= length - 4;) {
			auto blen = *reinterpret_cast<uint32_t const *>(original + i);
			i += 4;
			if (i + blen > length) return false;
			for (auto j = 0u; j < blen; ++j)
				input[p + j] = static_cast<uint8_t>(original[i + j] ^ WzKey::emsWzKey[j]);
			i += blen;
			p += blen;
		}
		length = p;

		return true;
	};

	std::copy(original, original + length, input.begin());
	if (!decompress() && (!decrypt() || !decompress())) {
		/*			std::cerr << "Unable to inflate: 0x" << std::setfill('0') << std::setw(2)
		<< std::hex << (unsigned)original[0] << " 0x" << std::setfill('0')
		<< std::setw(2) << std::hex << static_cast<unsigned>(original[1])
		<< std::endl;*/
		format = 2;
		format2 = 0;
		decompressed = size;
		std::fill(output.begin(), output.begin() + size, 0);
	}
	input.swap(output);

	struct color4444 {
		uint8_t b : 4;
		uint8_t g : 4;
		uint8_t r : 4;
		uint8_t a : 4;
	};
	static_assert(sizeof(color4444) == 2, "Your bitpacking sucks");
	struct color8888 {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};
	static_assert(sizeof(color8888) == 4, "Your bitpacking sucks");
	struct color565 {
		uint16_t b : 5;
		uint16_t g : 6;
		uint16_t r : 5;
	};
	static_assert(sizeof(color565) == 2, "Your bitpacking sucks");
	auto pixels4444 = reinterpret_cast<color4444 *>(input.data());
	auto pixels565 = reinterpret_cast<color565 *>(input.data());
	auto pixelsout = reinterpret_cast<color8888 *>(output.data());
	//Sanity check the sizes
	auto check = decompressed;

	switch (format) {
	case 1: check *= 2; break;
	case 2: break;
	case 513: check *= 2; break;
	case 1026: check *= 4; break;
	}
	auto pixels = width * height;
	switch (format2) {
	case 0: break;
	case 4: pixels /= 256; break;
	}
	if (check != pixels * 4) {
		std::cerr << "Size mismatch: " << std::dec << width << "," << height << "," << decompressed << "," << format << "," << format2 << std::endl;
		//throw std::runtime_error("halp!");
	}

	switch (format) {
	case 1:
		for (auto i = 0; i < pixels; ++i) {
			auto p = pixels4444[i];
			pixelsout[i] = { table4[p.r], table4[p.g], table4[p.b], table4[p.a]};
		}
		input.swap(output);
		break;
	case 2:

	
		//memcpy(pixelsout, input.data(), pixels);
		//input.swap(output);
		// Do nothing
		break;
	case 513:
		for (auto i = 0; i < pixels; ++i) {
			auto p = pixels565[i];
			pixelsout[i] = { table5[p.r], table6[p.g], table5[p.b], 255 };
		}
		input.swap(output);
		break;
	case 1026:
		//最新客户端包含这种纹理;
		//squish::DecompressImage(output.data(), width, height, input.data(), squish::kDxt3);
		//input.swap(output);
		break;
	default:
		throw std::runtime_error("Unknown image type!");
	}
	switch (format2) {
	case 0:
		// Do nothing
		break;
	case 4:
		//std::cerr << "Format2 of 4 at " << std::dec << index << std::endl;
		scale<16>(input, output, width, height);
		input.swap(output);
		break;
	default:
		//std::cerr << "Unknown image format2 of" << std::dec << static_cast<unsigned>(f2) << std::endl;
		throw std::runtime_error("Unknown image type!");
	}
	delete[] original;
	output.clear();
	return input;
}


