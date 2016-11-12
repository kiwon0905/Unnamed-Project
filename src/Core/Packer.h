#pragma once

#include <cstdint>
#include <vector>
#include <cassert>


template <int A, int B>
struct Power
{
	static const int value = A * Power<A, B - 1>::value;
};

template <int A>
struct Power<A, 0>
{
	static const int value = 1;
};

template<uint64_t val > struct Log2
{
	static const uint32_t result = Log2< val / 2 >::result + 1;
};

template<> struct Log2< 1 > 
{ 
	static const uint64_t result = 0;
};

template<uint64_t min, uint64_t max> struct BitsRequired
{
	static const uint32_t result = Log2<max - min>::result + 1;
};

class Packer
{
public:
	Packer();

	const void * getData();
	std::size_t getDataSize();

	template<int minInt, int maxInt, int res>
	void pack(float value)
	{
		const int32_t min = minInt * Power<10, res>::value;
		const int32_t max = maxInt * Power<10, res>::value;
		int32_t intValue = (int32_t)(value * Power<10, res>::value);
		pack<min, max>(intValue);
	}

	template <int32_t min, int32_t max>
	void pack(std::int32_t value)
	{
		assert(min < max && "min must be less than max");
		assert(value >= min && "value must be greater than or equal to min");
		assert(value <= max && "value must be less than or equal to max");
		const int bits = BitsRequired<min, max>::result;
		uint32_t unsignedValue = value - min;
		pack32(unsignedValue, bits);
	}

	template <uint32_t min, uint32_t max>
	void pack(std::uint32_t value)
	{
		assert(min < max && "min must be less than max");
		assert(value >= min && "value must be greater than or equal to min");
		assert(value <= max && "value must be less than or equal to max");
		const int bits = BitsRequired<min, max>::result;
		pack32(value, bits);
	}

	void pack(const std::string & data);
	void pack(const void * data, std::size_t size);

private:
	void align();
	void pack8(std::uint8_t data, std::size_t bits);
	void pack32(std::uint32_t data, std::size_t bits);
	void check(std::size_t size, std::size_t bits);
	std::vector<std::uint8_t> m_data;
	std::size_t m_bitPos;
};

class Unpacker
{
public:
	Unpacker(const void * data, std::size_t size);

	template<int minInt, int maxInt, int res>
	void unpack(float & data)
	{
		const int32_t min = minInt * Power<10, res>::value;
		const int32_t max = maxInt * Power<10, res>::value;
		int32_t intVal;
		unpack<min, max>(intVal);
		data = (float)intVal / Power<10, res>::value;
	}

	template <int32_t min, int32_t max>
	void unpack(std::int32_t & data)
	{
		assert(min < max && "min must be less than max");
		const int bits = BitsRequired<min, max>::result;
		uint32_t unsignedValue;
		unpack32(unsignedValue, bits);
		int32_t value = (int32_t)unsignedValue + min;
		data = value;
	}

	template <uint32_t min, uint32_t max>
	void unpack(std::uint32_t & data)
	{
		assert(min < max && "min must be less than max");
		const int bits = BitsRequired<min, max>::result;
		data = unpack32(bits);
	}

	void unpack(std::string & data);
	void unpack(void * data, std::size_t size);

private:
	void align();
	void unpack8(std::uint8_t & data, std::size_t bits);
	void unpack32(std::uint32_t & data, std::size_t bits);
	void check(std::size_t bits);
	const std::uint8_t * m_data;
	std::size_t m_size;
	std::size_t m_byteIndex;
	std::size_t m_bitsRead;
	std::size_t m_bitPos;
};
