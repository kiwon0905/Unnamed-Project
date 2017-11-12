#pragma once

#include <cstdint>
#include <vector>
#include <cassert>


#include <iostream>
#include <bitset>



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

	void pack(bool b);

	void pack(const std::string & s);

	template <typename F>
	std::enable_if_t<std::is_floating_point_v<F>> pack(F f, int res);

	template <typename E>
	std::enable_if_t<std::is_enum_v<E>> pack(E e);

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>> pack(T data);

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>> pack(T data);

private:
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

	bool unpack(bool & b);

	bool unpack(std::string & s);

	template <typename F>
	std::enable_if_t<std::is_floating_point_v<F>, bool> unpack(F & f, int res);

	template <typename E>
	std::enable_if_t<std::is_enum_v<E>, bool> unpack(E & e);

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool> unpack(T & data);

	template <typename T>
	std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, bool> unpack(T & data);

private:
	const std::uint8_t * m_data;
	std::size_t m_size;
	std::size_t m_readPos;
};

#include "Packer.inl"