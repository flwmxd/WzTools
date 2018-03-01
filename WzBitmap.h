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
#include "WzReader.h"
#include <vector>
class WzBitmap
{
public:
	WzBitmap();
	WzBitmap(int32_t height, int32_t width, uint32_t length,int32_t offset,int32_t format,uint8_t format2, std::shared_ptr<WzReader> reader);
	~WzBitmap();
	auto data()->std::vector<uint8_t>&;
	auto id()const->int32_t ;
	auto getWidth()const->int32_t;
	auto getHeight()const->int32_t;
	auto setWidth(int32_t width) -> void;
private:
	auto content() const->byte*;
public:
	uint32_t length;
	int32_t offset;
	int32_t format;
	int32_t height;
	int32_t width;
	uint8_t format2;
	std::shared_ptr<WzReader> reader;
	std::vector<uint8_t> input;
};

