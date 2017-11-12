#include "Packer.h"

#include <algorithm>

Packer::Packer():
	m_bitPos(0)
{
}

const void * Packer::getData() const
{
	if (m_data.empty())
		return nullptr;
	return m_data.data();
}

std::size_t Packer::getDataSize() const
{
	return m_data.size();
}

void Packer::pack(bool b)
{
	int intBool = static_cast<int>(b);
	pack(intBool);
}

void Packer::pack(const std::string & s)
{
	pack(s.size());
	std::size_t start = m_data.size();
	m_data.resize(start + s.size());
	std::memcpy(&m_data[start], s.data(), s.size());
}

Unpacker::Unpacker(const void * data, std::size_t size) :
	m_data(static_cast<const std::uint8_t*>(data)),
	m_size(size),
	m_readPos(0)
{
}

Unpacker::Unpacker():
	Unpacker(nullptr, 0)
{
}

void Unpacker::setData(const void * data, std::size_t size)
{
	m_data = static_cast<const std::uint8_t*>(data);
	m_size = size;
}

bool Unpacker::unpack(bool & b)
{
	int intBool;
	if (!unpack(intBool))
		return false;
	b = static_cast<bool>(intBool);
	return true;
}

bool Unpacker::unpack(std::string & s)
{
	std::size_t size;
	if (!unpack(size))
		return false;

	if (m_readPos + size > m_size)
		return false;

	s.clear();
	s.resize(size);
	std::memcpy(&s[0], &m_data[m_readPos], size);
	m_readPos += size;
	return true;
}