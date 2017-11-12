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

	void p()
	{
		for (char c : m_data)
			std::cout << std::bitset<8>(c) << " ";
		std::cout << "\n";
	}

	void pack_v(bool b)
	{
		int intBool = static_cast<int>(b);
		pack_v(intBool);
	}

	void pack_v(const std::string & s)
	{
		pack_v(s.size());
		std::size_t start = m_data.size();
		m_data.resize(start + s.size());
		std::memcpy(&m_data[start], s.data(), s.size());
	}

	template <typename F>
	std::enable_if_t<std::is_floating_point_v<F>> pack_v(F f, int res)
	{
		int intFloat = static_cast<int>(std::round(f * res));
		pack_v(intFloat);
	}

	template <typename E>
	std::enable_if_t<std::is_enum_v<E>> pack_v(E e)
	{
		std::underlying_type_t<E> intEnum = static_cast<std::underlying_type_t<E>>(e);
		//std::cout << "intenum: " << intEnum << "\n";
		pack_v(intEnum);
	}


	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>> pack_v(T data)
	{
		do
		{
			m_data.emplace_back();
			m_data.back() = data & 0x7f; //pack 7 bits
			data >>= 7;
			if (data)
				m_data.back() |= (1 << 7); //set extend bits
		} while (data);
	}

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>> pack_v(T data)
	{
		m_data.emplace_back();

		//set sign bit if negative
		if (data < 0)
			m_data.back() |= 0x40;

		std::make_unsigned_t<T> udata = std::abs(data);
		//pack 6 bits

		m_data.back() |= udata & 0x3f;
		udata >>= 6;
		while (udata)
		{
			m_data.back() |= 0x80;
			m_data.emplace_back();
			m_data.back() = udata & 0x7f;
			udata >>= 7;
		}
	}
	

private:
	//void pack8(std::uint8_t data, std::size_t bits);
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

	bool unpack_v(bool & b)
	{
		int intBool;
		if (!unpack_v(intBool))
			return false;
		b = static_cast<bool>(intBool);
		return true;
	}

	bool unpack_v(std::string & s)
	{
		std::size_t size;
		if (!unpack_v(size))
			return false;

		if (m_readPos + size > m_size)
			return false;

		s.clear();
		s.resize(size);
		std::memcpy(&s[0], &m_data[m_readPos], size);
		m_readPos += size;
		return true;
	}

	template <typename F>
	std::enable_if_t<std::is_floating_point_v<F>, bool> unpack_v(F & f, int res)
	{
		int intFloat;
		if (!unpack_v(intFloat))
			return false;
		f = static_cast<float>(intFloat) / res;
		return true;
	}

	template <typename E>
	std::enable_if_t<std::is_enum_v<E>, bool> unpack_v(E & e)
	{
		std::underlying_type_t<E> intEnum;
		if (!unpack_v(intEnum))
			return false;
		e = static_cast<E>(intEnum);
		return true;
	}

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool> unpack_v(T & data)
	{
		data = 0;

		int i = 0;
		do
		{
			if (m_readPos >= m_size)
				return false;

			data |= (T(m_data[m_readPos] & 0x7f) << (7 * i));
			if (!(m_data[m_readPos] & 0x80))
				break;

			m_readPos++;
			++i;

		} while (true);
		m_readPos++;
		return true;
	}

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, bool> unpack_v(T & data)
	{
		if (m_readPos >= m_size)
			return false;

		int sign = (m_data[m_readPos] >> 6 & 1);

		std::make_unsigned_t<T> udata = 0;

		//read 6 bits
		udata = m_data[m_readPos] & 0x3f;

		int i = 0;
		while (m_data[m_readPos] & 0x80)
		{
			m_readPos++;
			if (m_readPos >= m_size)
				return false;

			udata |= (std::make_unsigned_t<T>(m_data[m_readPos] & 0x7f) << (6 + 7 * i));
			++i;
		}

		data = udata;
		if (sign)
			data = -data;
		m_readPos++;
		return true;
	}



private:

	const std::uint8_t * m_data;
	std::size_t m_size;

	std::size_t m_readPos;
};


//#include "Packer.inl"