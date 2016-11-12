#include "Packer.h"

Packer::Packer()
{
}

void Packer::align()
{
	if (m_bitPos > 0)
	{
		unsigned leftOver = 8 - m_bitPos;
		pack8(0, leftOver);
	}
}

void Packer::pack32(std::uint32_t value, unsigned bits)
{
	std::uint8_t * p = reinterpret_cast<std::uint8_t *>(&value);
	while (bits >= 8)
	{
		pack8(*p, 8);
		p++;
		bits -= 8;
	}
	pack8(*p, bits);
}

void Packer::pack8(std::uint8_t value, unsigned bits)
{
	assert(bits >= 0);
	assert(bits <= 8);

	if (bits == 0)
		return;

	if (m_bitPos == 0)
		m_data.push_back(0);
	value &= (uint8_t(1) << bits) - 1;


	std::uint8_t a = value;
	a = a << (8 - bits);
	a = a >> m_bitPos;
	m_bitPos += bits;
	m_data.back() |= a;



	if (m_bitPos > 8)
	{
		m_bitPos -= 8;
		m_data.push_back(0);
		std::uint8_t b = value;
		b = b << (8 - m_bitPos);
		m_data.back() |= b;
	}
}

void Packer::pack(const std::string & s)
{
	uint32_t length = static_cast<uint32_t>(s.size());
	pack32(length, 32);

	if (length)
		pack(s.data(), length);

}

void Packer::pack(const void * data, std::size_t size)
{
	align();
	std::size_t start = m_data.size();
	m_data.resize(start + size);
	std::memcpy(&m_data[start], data, size);
}

const std::uint8_t * Packer::getData()
{
	return &m_data[0];
}

std::size_t Packer::getDataSize()
{
	return m_data.size();
}

Unpacker::Unpacker(const void * data, std::size_t size):
	m_data(static_cast<const uint8_t *>(data)),
	m_size(size)
{
}

void Unpacker::unpack(std::string & s)
{
	uint32_t length;
	unpack32(length, 32);
	s.clear();
	s.resize(length);

	unpack(reinterpret_cast<uint8_t*>(&s[0]), length);
}

void Unpacker::unpack(void * data, std::size_t size)
{
	align();
	assert(m_totalBitsRead + 8 * size <= 8 * m_size && "attempt to read more than available");
	std::memcpy(data, &m_data[m_byteIndex], size);
	m_totalBitsRead += 8 * size;
	m_byteIndex += size;
}

void Unpacker::align()
{
	if (m_bitPos > 0)
	{
		unsigned leftOver = 8 - m_bitPos;
		std::uint8_t temp;
		unpack8(temp, leftOver);
	}
}

void Unpacker::unpack8(std::uint8_t & data, unsigned bits)
{
	assert(m_totalBitsRead + bits <= 8 * m_size && "attempt to unpack8 more than available");
	m_totalBitsRead += bits;

	if (m_bitPos + bits >= 8)
	{
		m_bitPos += bits;
		m_bitPos -= 8;
		std::uint8_t first = m_data[m_byteIndex];
		first = first & (1 << (bits - m_bitPos)) - 1;
		first = first << m_bitPos;
		m_byteIndex++;
		std::uint8_t second = m_data[m_byteIndex];
		second = second >> (8 - m_bitPos);

		data = first | second;
	}
	else
	{
		std::uint8_t val = m_data[m_byteIndex];
		val = val >> (8 - m_bitPos - bits);
		m_bitPos += bits;
		data = val & (1 << bits) - 1;
	}
}

void Unpacker::unpack32(std::uint32_t & data, unsigned bits)
{
	std::uint32_t value = 0;
	std::uint8_t * p = reinterpret_cast<std::uint8_t*>(&value);
	while (bits >= 8)
	{
		std::uint8_t val;
		unpack8(val, 8);
		*p |= val;
		p++;
		bits -= 8;
	}
	std::uint8_t val;
	unpack8(val, bits);
	*p |= val;
	data = value;
}