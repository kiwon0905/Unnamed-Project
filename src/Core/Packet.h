#pragma once
#include "Protocol.h"

#include <vector>
#include <type_traits>


class Packet
{
public:
	void write(const void * data, std::size_t size);

	template <typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, Packet &> operator<<(T data);
	template <typename T>
	std::enable_if_t<std::is_enum<T>::value, Packet &> operator<<(T data);
	Packet & operator<<(const std::string & data);
	
	template <typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, Packet &> operator>>(T & data);
	template <typename T>
	std::enable_if_t<std::is_enum<T>::value, Packet &> operator>>(T & data);
	Packet & operator>>(std::string & data);

	void clear();
	const void * getData() const;
	std::size_t getDataSize() const;
private:
	std::vector<char> m_data;
	std::size_t m_readPos = 0;
};

template<typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, Packet&> Packet::operator<<(T data)
{
	write(&data, sizeof(data));
	return *this;
}

template <typename T>
inline std::enable_if_t<std::is_enum<T>::value, Packet &> Packet::operator<<(T data)
{
	write(&data, sizeof(data));
	return *this;
}

template<typename T>
inline std::enable_if_t<std::is_arithmetic<T>::value, Packet&> Packet::operator>>(T & data)
{
	data = *reinterpret_cast<const T *>(&m_data[m_readPos]);
	m_readPos += sizeof(data);
	return *this;
}

template <typename T>
inline std::enable_if_t<std::is_enum<T>::value, Packet &> Packet::operator>>(T & data)
{
	data = *reinterpret_cast<const T *>(&m_data[m_readPos]);
	m_readPos += sizeof(data);
	return *this;
}
