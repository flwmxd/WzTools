#pragma once
#include <iostream>
#include <memory>
#include <vector>
typedef unsigned char byte;
class WzAudio;
class WzBitmap;
class WzNode;
class WzReader;
struct Data
{

	int64_t ireal;
	double dreal;
	std::string str;
	int32_t vector[2] = {0,0};
	struct {
		uint32_t length;
		uint32_t offset;
		uint16_t width;
		uint16_t height;
		uint32_t format;
		uint8_t format2;
	}bitmap;
	struct {
		uint32_t offset;
		uint32_t length;
	}audio;
	
	bool use = false;
};

class ByteArrayOutputStream {
private:
	byte * bytes;
	int size = 32;
	int position = 0;
	int length = 0;
public:
	ByteArrayOutputStream() {
		bytes = new byte[size];
	}
	ByteArrayOutputStream(int size) {
		size = size;
		bytes = new byte[size];
	}

	~ByteArrayOutputStream() {
		delete[]bytes;
	}

	void write(byte * b, int offset, int count) {
		//position = offset;
		for (int i = offset; i < offset + count; i++)
		{
			write(b[i]);
		}
	}

	void write(byte b) {
		if (position >= size) {
			int presize = size;
			size += 10;
			byte * temp = bytes;
			bytes = new byte[size];
			memcpy(bytes, temp, presize * sizeof(byte));
			delete[] temp;
		}
		this->bytes[position] = b;
		if (position < length) {
			position++;
		}
		else {
			position++;
			length++;
		}
	}

	byte * toArray() {
		byte * b = new byte[length];
		memcpy(b, bytes, length);
		return b;
	}
	void setPosition(int position) {
		this->position = position;
	}
	int getLength()const {
		return length;
	}
};


class WzTools {
public:
	static std::string types[6];
	static auto split(const std::string &str, std::vector<std::string> &ret, std::string sep, bool allowNull) -> int;
	static auto endWith(const std::string &str, const std::string &endStr)->bool;
	static auto getIndexInTypes(std::string &str) -> int;
};