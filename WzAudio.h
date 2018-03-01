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

class WzAudio
{
public:
	WzAudio(std::shared_ptr<WzReader> reader, uint32_t offset, uint32_t len,int32_t header);
	WzAudio() = default;
	auto getData() ->std::shared_ptr<std::vector<uint8_t>>&;
	auto getAudioData()->byte*;
	auto getId()->size_t;
	auto getLength()->uint32_t;
	auto getInformation() -> void;
	auto getWaveHeader()->byte*;
	auto isPcm() -> bool;
	auto getChannel()->int32_t;
	auto getRate()->int32_t;
	auto getBits()->int32_t;
private:
	int32_t channel;
	int32_t rate;
	int32_t bits;
private:

	std::shared_ptr<std::vector<uint8_t>> data;
	//byte * data = nullptr;
	uint32_t length = 0;
	int32_t header = 0;
	std::shared_ptr<WzReader> reader;
	uint32_t offset = 0;
    static int32_t bitRates[2][3][16];
    
    static int32_t frequencies[][4];
    
    static int32_t samples_per_frames[][3];
    
    static int32_t coefficients[][3];
    
    static int32_t slots[3] ;
};

