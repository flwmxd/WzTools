#pragma once
#include <cstdint>
#include <cstddef>
#include <memory>
#include "WzTools.h"

class WzAudio
{
public:
	~WzAudio();
	WzAudio(std::shared_ptr<WzReader> reader, uint32_t offset, uint32_t len);
	WzAudio() = default;
	WzAudio(WzAudio const &) = default;
	WzAudio & operator=(WzAudio const &) = default;
	auto getData() ->byte*;
private:
	byte * m_data = nullptr;
	uint32_t length = 0;
	std::shared_ptr<WzReader> reader;
	uint32_t offset = 0;
};

