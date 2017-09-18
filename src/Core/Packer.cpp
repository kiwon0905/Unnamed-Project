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

void Packer::pack(bool data)
{
	pack8(static_cast<std::uint8_t>(data), 1);
}

void Packer::pack(const std::string & data)
{
	std::uint32_t length = static_cast<std::uint32_t>(data.size());
	pack(length, 32);
	if (length)
		pack(data.data(), length);

}

void Packer::pack(const void * data, std::size_t size)
{
	align();
	std::size_t start = m_data.size();
	m_data.resize(start + size);
	std::memcpy(&m_data[start], data, size);

	std::cout << "size: " << size << "\n";
}

void Packer::align()
{
	if (m_bitPos)
		pack8(0, 8 - m_bitPos);
}

void Packer::pack8(std::uint8_t data, std::size_t bits)
{
	if (bits == 0)
		return;

	assert(bits <= 8);
	
	if (m_bitPos == 0)
		m_data.push_back(0);

	data &= (std::uint8_t(1) << bits) - 1;

	std::size_t firstBits = std::min(8 - m_bitPos, bits);
	std::uint8_t firstVal = data << (8 - bits);
	firstVal >>= m_bitPos;
	m_data.back() |= firstVal;
	
	m_bitPos += bits;
	if (m_bitPos >= 8)
		m_bitPos -= 8;

	bits -= firstBits;
	if (bits)
	{
		std::uint8_t secondVal = data & (1 << bits) - 1;
		secondVal <<= (8 - bits);
		m_data.push_back(0);
		m_data.back() |= secondVal;
	}
}

Unpacker::Unpacker(const void * data, std::size_t size) :
	m_data(static_cast<const std::uint8_t*>(data)),
	m_size(size),
	m_byteIndex(0),
	m_bitsRead(0),
	m_bitPos(0)
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
	m_byteIndex = 0;
	m_bitsRead = 0;
	m_bitPos = 0;
}

void Unpacker::unpack(bool & data)
{
	std::uint8_t val;
	unpack8(val, 1);
	data = (val != 0);
}

void Unpacker::unpack(std::string & data)
{
	std::uint32_t length;
	unpack(length, 32);
	data.clear();
	data.resize(length);
	unpack(reinterpret_cast<std::uint8_t*>(&data[0]), length);
}

void Unpacker::unpack(void * data, std::size_t size)
{
	align();
	check(8 * size);
	std::memcpy(data, &m_data[m_byteIndex], size);
	m_bitsRead += 8 * size;
	m_byteIndex += size;
}

void Unpacker::align()
{
	if (m_bitPos)
	{
		std::uint8_t a;
		unpack8(a, 8 - m_bitPos);
	}
}

void Unpacker::unpack8(std::uint8_t & data, std::size_t bits)
{
	if (bits == 0)
		return;

	assert(bits <= 8);
	check(bits);

	data = 0;
	std::size_t firstBits = std::min(8 - m_bitPos, bits);
	data = m_data[m_byteIndex] >> (8 - firstBits - m_bitPos);
	data &= (std::uint8_t(1) << firstBits) - 1;
	

	m_bitPos += bits;
	if (m_bitPos >= 8)
	{
		m_bitPos -= 8;
		m_byteIndex++;
	}

	bits -= firstBits;

	if (bits > 0)
	{
		data <<= bits;
		std::uint8_t secondVal = m_data[m_byteIndex];
		secondVal &= 0xFF << (8 - bits);
		secondVal >>= (8 - bits);
		data |= secondVal;
	}
}

void Unpacker::check(std::size_t bits)
{
	assert(m_bitsRead + bits <= 8 * m_size && "attempt to read more than available");
}
