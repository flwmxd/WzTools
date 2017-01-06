#include "WzAudio.h"
#include "WzReader.h"


WzAudio::WzAudio(std::shared_ptr<WzReader> r, uint32_t o,uint32_t len) :reader(r),offset(o), length(len)
{
}

auto WzAudio::getData() -> byte *
{
	/*if (m_data != nullptr) {
		return m_data;
	}*/
	reader->setPosition(offset);
	return/* m_data = */reader->readBytes(this->length);
}


WzAudio::~WzAudio()
{
	/*if (m_data != nullptr) {
		delete[] m_data;
	}*/
}
