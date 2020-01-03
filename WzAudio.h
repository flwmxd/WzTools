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
#include <cstdint>
#include <cstddef>
#include <memory>
#include "WzTools.h"
class WzReader;

class WzAudio
{
public:
	WzAudio(std::shared_ptr<WzReader> reader, uint32_t offset, uint32_t len, int32_t header);
	WzAudio() = default;
	auto getData() -> std::shared_ptr<std::vector<uint8_t>>&;
	auto getAudioData() -> const int8_t*;
	auto getInformation() -> void;
	auto getWaveHeader() -> int8_t*;

	inline auto getId() const { return offset; };
	inline auto getLength() const{ return length; };
	inline auto isPcm() const { return 0x12 == header; };
	inline auto getChannel() const{ return channel; };
	inline auto getRate() const{ return rate; };
	inline auto getBits() const{ return bits; };

private:
	int32_t channel;
	int32_t rate;
	int32_t bits;
	int32_t header = 0;
	uint32_t offset = 0;
	uint32_t length = 0;
	std::shared_ptr<std::vector<uint8_t>> data;
	std::shared_ptr<WzReader> reader;
	const int8_t* buff = nullptr;
private:
	static int32_t bitRates[2][3][16];
	static int32_t frequencies[][4];
	static int32_t samples_per_frames[][3];
	static int32_t coefficients[][3];
	static int32_t slots[3];
};
