template <typename T>
std::enable_if_t<std::is_integral_v<T>> Packer::pack(T data, std::size_t bits)
{
	std::uint8_t * p = reinterpret_cast<std::uint8_t*>(&data);
	while (bits >= 8)
	{
		pack8(*p, 8);
		p++;
		bits -= 8;
	}
	pack8(*p, bits);
}

template <std::int8_t min, std::int8_t max>
void Packer::pack(std::int8_t data)
{
	const int bits = BitsRequired<min, max>::result;
	uint8_t unsignedValue = data - min;
	pack(unsignedValue, bits);
}

template <std::uint8_t min, std::uint8_t max>
void Packer::pack(std::uint8_t data)
{
	const int bits = BitsRequired<min, max>::result;
	pack(data, bits);
}

template <std::int16_t min, std::int16_t max>
void Packer::pack(std::int16_t data)
{
	const int bits = BitsRequired<min, max>::result;
	uint16_t unsignedValue = data - min;
	pack(unsignedValue, bits);
}

template <std::uint16_t min, std::uint16_t max>
void Packer::pack(std::uint16_t data)
{
	const int bits = BitsRequired<min, max>::result;
	pack(data, bits);
}

template <int32_t min, int32_t max>
void Packer::pack(std::int32_t data)
{
	const int bits = BitsRequired<min, max>::result;
	uint32_t unsignedValue = data - min;
	pack(unsignedValue, bits);
}

template <uint32_t min, uint32_t max>
void Packer::pack(std::uint32_t data)
{
	const int bits = BitsRequired<min, max>::result;
	pack(data, bits);
}

template<std::int64_t min, std::int64_t max>
inline void Packer::pack(std::int64_t data)
{
	const int bits = BitsRequired<min, max>::result;
	uint64_t unsignedValue = data - min;
	pack(unsignedValue, bits);
}

template<std::uint64_t min, std::uint64_t max>
inline void Packer::pack(std::uint64_t data)
{
	const int bits = BitsRequired<min, max>::result;
	pack(data, bits);
}

template<typename T>
inline std::enable_if_t<std::is_enum<T>::value> Packer::pack(T data)
{
	const int bits = BitsRequired<0, static_cast<std::uint64_t>(T::COUNT)>::result;
	pack(static_cast<std::uint32_t>(data), bits);
}

template<typename T>
inline std::enable_if_t<std::is_integral_v<T>> Unpacker::unpack(T & data, std::size_t bits)
{
	T value = 0;
	std::uint8_t * p = reinterpret_cast<std::uint8_t*>(&value);
	while (bits >= 8)
	{
		std::uint8_t val;
		unpack8(val, 8);
		*p |= val;
		p++;
		bits -= 8;
	}
	std::uint8_t val = 0;
	unpack8(val, bits);
	*p |= val;
	data = value;
}


template <std::int8_t min, std::int8_t max>
void Unpacker::unpack(std::int8_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	uint8_t unsignedValue;
	unpack(unsignedValue, bits);
	int8_t value = (int8_t)unsignedValue + min;
	data = value;
}

template <std::uint8_t min, std::uint8_t max>
void Unpacker::unpack(std::uint8_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	unpack(data, bits);
}

template <std::int16_t min, std::int16_t max>
void Unpacker::unpack(std::int16_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	uint16_t unsignedValue;
	unpack(unsignedValue, bits);
	int16_t value = (int16_t)unsignedValue + min;
	data = value;
}

template <std::uint16_t min, std::uint16_t max>
void Unpacker::unpack(std::uint16_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	unpack(data, bits);
}

template <int32_t min, int32_t max>
void Unpacker::unpack(std::int32_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	uint32_t unsignedValue;
	unpack(unsignedValue, bits);
	int32_t value = (int32_t)unsignedValue + min;
	data = value;
}

template <uint32_t min, uint32_t max>
void Unpacker::unpack(std::uint32_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	unpack(data, bits);
}

template<std::int64_t min, std::int64_t max>
inline void Unpacker::unpack(std::int64_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	uint64_t unsignedValue;
	unpack(unsignedValue, bits);
	int64_t value = (int64_t)unsignedValue + min;
	data = value;
}

template<std::uint64_t min, std::uint64_t max>
inline void Unpacker::unpack(std::uint64_t & data)
{
	const int bits = BitsRequired<min, max>::result;
	unpack(data, bits);
}

template<typename T>
inline std::enable_if_t<std::is_enum<T>::value> Unpacker::unpack(T & data)
{
	std::uint32_t intEnum;
	unpack<0, static_cast<std::uint32_t>(T::COUNT)>(intEnum);
	data = static_cast<T>(intEnum);
}

template <typename T>
inline void Unpacker::peek(T & data, std::size_t bits)
{
	std::size_t prevByteIndex = m_byteIndex;
	std::size_t prevBitsRead = m_bitsRead;
	std::size_t prevBitPos = m_bitPos;
	unpack(data, bits);
	m_byteIndex = prevByteIndex;
	m_bitsRead = prevBitsRead;
	m_bitPos = prevBitPos;
}