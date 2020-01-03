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

#include "WzAudio.h"
#include "WzReader.h"
#include "WzNode.h"
#include <memory.h>
int32_t WzAudio::bitRates[2][3][16] = {
	//mpeg1
	{
		{
			0,
			32,
			64,
			96,
			128,
			160,
			192,
			224,
			256,
			288,
			320,
			352,
			384,
			416,
			448,
			0,
		}, // layer1
		{
			0,
			32,
			48,
			56,
			64,
			80,
			96,
			112,
			128,
			160,
			192,
			224,
			256,
			320,
			384,
			0,
		}, // layer2
		{
			0,
			32,
			40,
			48,
			56,
			64,
			80,
			96,
			112,
			128,
			160,
			192,
			224,
			256,
			320,
			0,
		}, // layer3
	},
	//mpeg2 & 2.5
	{
		{
			0,
			32,
			48,
			56,
			64,
			80,
			96,
			112,
			128,
			144,
			160,
			176,
			192,
			224,
			256,
			0,
		}, // layer1
		{
			0,
			8,
			16,
			24,
			32,
			40,
			48,
			56,
			64,
			80,
			96,
			112,
			128,
			144,
			160,
			0,
		}, // layer2
		{
			0,
			8,
			16,
			24,
			32,
			40,
			48,
			56,
			64,
			80,
			96,
			112,
			128,
			144,
			160,
			0,
		}, // layer3
	}};

int32_t WzAudio::frequencies[][4] = {
	{
		11025,
		12000,
		8000,
		0,
	}, // mpeg2.5
	{
		0,
		0,
		0,
		0,
	}, // reserved
	{
		22050,
		24000,
		16000,
		0,
	}, // mpeg2
	{
		44100,
		48000,
		32000,
		0,
	}, // mpeg1
};

int32_t WzAudio::samples_per_frames[][3] = {
	//mpeg1
	{
		384,  // layer1
		1152, // layer2
		1152, // layer3
	},
	//mpeg2 & 2.5
	{
		384,  // layer1
		1152, // layer2
		576,  // layer3
	},
};
int32_t WzAudio::coefficients[][3] = {
	//mpeg1
	{
		12,  // layer1
		144, // layer2
		144, // layer3
	},
	//mpeg2 & 2.5
	{
		12,  // layer1
		144, // layer2
		72,  // layer3
	},
};
int32_t WzAudio::slots[3] = {
	4, // layer1
	1, // layer2
	1, // layer3
};

WzAudio::WzAudio(std::shared_ptr<WzReader> r, uint32_t o, uint32_t len, int32_t header) : header(header), offset(o),
																						  length(len), reader(r)
{
}

auto WzAudio::getInformation() -> void
{
	if (!isPcm()) //MP3
	{
		int8_t *raw = nullptr;
		std::unique_ptr<int8_t[]> rawPtr = nullptr;
		int32_t tag = 0;
		int32_t id3 = 0;
		if (128 < length)
		{
			reader->setPosition(offset + length - 128);

			rawPtr = reader->readBytes(3);
			raw = rawPtr.get();
			if (0x54 == raw[0] && 0x41 == raw[1] && 0x47 == raw[2])
				tag = 128;
		}
		reader->setPosition(offset);

		rawPtr = reader->readBytes(4);
		raw = rawPtr.get();
		if (0x49 == raw[0] && 0x44 == raw[1] && 0x33 == raw[2])
		{
			reader->setPosition(reader->getPosition() + 2);

			rawPtr = reader->readBytes(4);
			raw = rawPtr.get();
			id3 = 10 + (((raw[0] & 0x7f) << 21) + ((raw[1] & 0x7f) << 14) + ((raw[2] & 0x7f) << 7) + (raw[3] & 0x7f));

			reader->setPosition(offset + id3);

			rawPtr = reader->readBytes(4);
			raw = rawPtr.get();
		}

		while (0xff == raw[0] && 0xe0 == (raw[1] & 0xe0) && 0xf0 != (raw[2] & 0xf0))
		{
			int32_t version;
			int32_t layer;
			int32_t crc;
			int32_t bitrate;
			int32_t frequency;
			int32_t padding;
			int32_t framesize; /*
							int framescount;*/
			int32_t songLength;

			// ---vv--- (0 = mpeg2.5, 1 = reserved, 2 = mpeg2, 3 = mpeg1)
			version = (raw[1] >> 3) & 3;

			if (1 == version)
				break;

			// -----ll- (0 = reserved, 1 = layer3, 2 = layer2, 3 = layer1)
			// 3 - layer
			layer = 3 - ((raw[1] >> 1) & 3);

			if (3 == layer)
				break;

			// -------c
			crc = raw[1] & 1;

			// bbbb----
			bitrate = (raw[2] >> 4) & 15;

			if (0 == bitrate)
				break;

			bits = bitrate = bitRates[3 == version ? 0 : 1][layer][bitrate];

			// ----ff--
			frequency = (raw[2] >> 2) & 3;

			if (3 == frequency)
				break;

			frequency = frequencies[version][frequency];

			// ------p-
			padding = (raw[2] >> 1) & 1;

			framesize = (coefficients[3 == version ? 0 : 1][layer] * bitrate * 1000 / frequency + padding) * slots[layer];

			// song length...
			songLength = (int32_t)(((length - tag - id3) / bitrate) * 8.0);

			rate = frequency;

			break;
			//	return string.Format("{0:d2}:{1:d2}.{2,-4:d} {3,3:d}kbps {4:g}khz", songLength / 60000, (songLength / 1000) % 60, songLength % 1000, bitrate, frequency / 1000.0);
		}
	}
	else
	{
		int8_t *raw = getWaveHeader();
		int16_t channel;
		memcpy(&channel, raw + 2, sizeof(int16_t));
		this->channel = channel;

		int32_t f;
		memcpy(&f, raw + 4, sizeof(int32_t));
		double frequency = f / 1000.0;
		int16_t bitrate;
		memcpy(&bitrate, raw + 0x0e, sizeof(int16_t)); //14
		int32_t ms = (int)((length / bitrate) * 8.0 / frequency);

		//return string.Format("{0:d2}:{1:d2}.{2,-4:d} {3,3:d}kbps {4:g}khz", ms / 60000, (ms / 1000) % 60, ms % 1000, (int)(frequency * bitrate), frequency) + " : PCM";
		if (raw)
		{
			delete[] raw;
		}
	}
}

auto WzAudio::getWaveHeader() -> int8_t *

{
	reader->setPosition(offset - header);
	if (1 == reader->readShort())
	{
		reader->setPosition(offset - header);
		return reader->readBytes(0x10).release();
	}
	reader->setPosition(offset - header);
	return reader->decryptBytes(reader->readBytes(0x10).release(), 0x10);
}

auto WzAudio::getData() -> std::shared_ptr<std::vector<uint8_t>> &

{
	if (data != nullptr && data->size() > 0)
	{
		return data;
	}
	reader->setPosition(offset);
	auto dataPtr = reader->readBytes(this->length);
	data = std::make_shared<std::vector<uint8_t>>();
	data->insert(data->begin(), dataPtr.get(), dataPtr.get() + length);
	//delete[] dataPtr;
	return data;
}

auto WzAudio::getAudioData() -> const int8_t *
{
	if (reader == nullptr)
	{
		return nullptr;
	}
	if (buff == nullptr) {
		buff = reader->getOffset(offset);
	}
	/*reader->setPosition(offset);
	auto ptr = reader->readBytes(this->length);
	return ptr.release();*/
	return buff;
}
