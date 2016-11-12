#include "Packet.h"

#include <cstdint>

void Packet::write(const void * data, std::size_t size)
{
	if (data && size)
	{
		std::size_t begin = m_data.size();
		m_data.resize(begin + size);
		std::memcpy(&m_data[begin], data, size);
	}
}

Packet & Packet::operator<<(const std::string & data)
{
	std::uint32_t length = data.size();
	*this << length;
	write(data.c_str(), length * sizeof(std::string::value_type));
	return *this;
}

Packet & Packet::operator>>(std::string & data)
{
	std::uint32_t length = 0;
	*this >> length;
	data.clear();
	if (length)
	{
		data.assign(&m_data[m_readPos], length);
		m_readPos += length;
	}
	return *this;
}

void Packet::clear()
{
	m_data.clear();
	m_readPos = 0;
}

const void * Packet::getData() const
{
	return m_data.data();
}

std::size_t Packet::getDataSize() const
{
	return m_data.size();
}
