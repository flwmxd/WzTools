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
	/*
	 *	RGBA
	 */
	auto png()->byte*;
	/*
	 *	BGRA
	 */
	auto data()->std::vector<uint8_t>;
	auto id()const->int32_t ;
	auto getWidth()const->int32_t;
	auto getHeight()const->int32_t;
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
	auto render(byte *)->byte*;
	unsigned long deCompressLen;

};

