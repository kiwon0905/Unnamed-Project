#pragma once

#include <cstdint>
#include <vector>
#include <cassert>


#include <iostream>
#include <bitset>

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
	Packer(const Packer &) = delete;
	Packer & operator=(const Packer &) = delete;

	const void * getData() const;
	std::size_t getDataSize() const;

	template <typename T>
	std::enable_if_t<std::is_integral_v<T>> pack(T data, std::size_t bits);
	template <std::int8_t min = (std::numeric_limits<std::int8_t>::min)(), std::int8_t max = (std::numeric_limits<std::int8_t>::max)()>
	void pack(std::int8_t data);
	template <std::uint8_t min = (std::numeric_limits<std::uint8_t>::min)(), std::uint8_t max = (std::numeric_limits<std::uint8_t>::max)()>
	void pack(std::uint8_t data);
	template <std::int16_t min = (std::numeric_limits<std::int16_t>::min)(), std::int16_t max = (std::numeric_limits<std::int16_t>::max)()>
	void pack(std::int16_t data);
	template <std::uint16_t min = (std::numeric_limits<std::uint16_t>::min)(), std::uint16_t max = (std::numeric_limits<std::uint16_t>::max)()>
	void pack(std::uint16_t data);
	template <std::int32_t min = (std::numeric_limits<std::int32_t>::min)(), std::int32_t max = (std::numeric_limits<std::int32_t>::max)()>
	void pack(std::int32_t data);
	template <std::uint32_t min = (std::numeric_limits<std::uint32_t>::min)(), std::uint32_t max = (std::numeric_limits<std::uint32_t>::max)()>
	void pack(std::uint32_t data);
	template <std::int64_t min = (std::numeric_limits<std::int64_t>::min)(), std::int64_t max = (std::numeric_limits<std::int64_t>::max)()>
	void pack(std::int64_t data);
	template <std::uint64_t min = (std::numeric_limits<std::uint64_t>::min)(), std::uint64_t max = (std::numeric_limits<std::uint64_t>::max)()>
	void pack(std::uint64_t data);
	void pack(bool data);
	void pack(const std::string & data);
	void pack(const void * data, std::size_t size);
	template <typename T>
	std::enable_if_t<std::is_enum<T>::value> pack(T data);

	void align();


	std::size_t getCurrentBitPos() { return m_bitPos; }
	void p()
	{
		for (char c : m_data)
			std::cout << std::bitset<8>(c) << " ";
		std::cout << "\n";
	}
private:
	void pack8(std::uint8_t data, std::size_t bits);
	std::vector<std::uint8_t> m_data;
	std::size_t m_bitPos;
};

class Unpacker
{
public:
	Unpacker();
	Unpacker(const void * data, std::size_t size);
	Unpacker(const Unpacker &) = delete;
	Unpacker & operator=(const Unpacker &) = delete;

	void setData(const void * data, std::size_t size);

	template <typename T>
	std::enable_if_t<std::is_integral_v<T>> unpack(T & data, std::size_t bits);
	template <std::int8_t min = (std::numeric_limits<std::int8_t>::min)(), std::int8_t max = (std::numeric_limits<std::int8_t>::max)()>
	void unpack(std::int8_t & data);
	template <std::uint8_t min = (std::numeric_limits<std::uint8_t>::min)(), std::uint8_t max = (std::numeric_limits<std::uint8_t>::max)()>
	void unpack(std::uint8_t & data);
	template <std::int16_t min = (std::numeric_limits<std::int16_t>::min)(), std::int16_t max = (std::numeric_limits<std::int16_t>::max)()>
	void unpack(std::int16_t & data);
	template <std::uint16_t min = (std::numeric_limits<std::uint16_t>::min)(), std::uint16_t max = (std::numeric_limits<std::uint16_t>::max)()>
	void unpack(std::uint16_t & data);
	template <std::int32_t min = (std::numeric_limits<std::int32_t>::min)(), std::int32_t max = (std::numeric_limits<std::int32_t>::max)()>
	void unpack(std::int32_t & data);
	template <std::uint32_t min = (std::numeric_limits<std::uint32_t>::min)(), std::uint32_t max = (std::numeric_limits<std::uint32_t>::max)()>
	void unpack(std::uint32_t & data);
	template <std::int64_t min = (std::numeric_limits<std::int64_t>::min)(), std::int64_t max = (std::numeric_limits<std::int64_t>::max)()>
	void unpack(std::int64_t & data);
	template <std::uint64_t min = (std::numeric_limits<std::uint64_t>::min)(), std::uint64_t max = (std::numeric_limits<std::uint64_t>::max)()>
	void unpack(std::uint64_t & data);
	void unpack(bool & data);
	void unpack(std::string & data);
	void unpack(void * data, std::size_t size);
	template <typename T>
	std::enable_if_t<std::is_enum<T>::value> unpack(T & data);
	
	template <typename T>
	void peek(T & data, std::size_t bits);
	void align();
private:
	void unpack8(std::uint8_t & data, std::size_t bits);
	void check(std::size_t bits);
	const std::uint8_t * m_data;
	std::size_t m_size;
	std::size_t m_byteIndex;
	std::size_t m_bitsRead;
	std::size_t m_bitPos;
};


#include "Packer.inl"