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
#include "WzTools.h"
#include <vector>
class WzReader;
class WzBitmap
{
public:
	WzBitmap() = default;
	WzBitmap(int32_t height, int32_t width, uint32_t length, uint32_t offset, uint32_t format, uint8_t format2,
	         std::shared_ptr<WzReader> reader);
	~WzBitmap();
	inline auto id() const { return offset; };
	inline auto getWidth() const  { return width; };
	inline auto getHeight() const { return height; };
	auto data() -> std::vector<uint8_t>&;
	auto setWidth(int32_t width) -> void;
private:
	auto content() const -> std::unique_ptr<int8_t[]>;
public:
	uint8_t format2 = 0;
	int32_t height = 0;
	int32_t width = 0;
	uint32_t length = 0;
	uint32_t offset = 0;
	uint32_t format = 0;

	std::shared_ptr<WzReader> reader = nullptr;
	std::vector<uint8_t> input;
	const int8_t * buff = nullptr;
};
