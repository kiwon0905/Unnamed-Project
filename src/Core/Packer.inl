template <typename F>
std::enable_if_t<std::is_floating_point_v<F>> Packer::pack(F f, int res)
{
	int intFloat = static_cast<int>(std::round(f * res));
	pack(intFloat);
}

template <typename E>
std::enable_if_t<std::is_enum_v<E>> Packer::pack(E e)
{
	std::underlying_type_t<E> intEnum = static_cast<std::underlying_type_t<E>>(e);
	pack(intEnum);
}

template <typename T>
std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>> Packer::pack(T data)
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
std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>> Packer::pack(T data)
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

template <typename F>
std::enable_if_t<std::is_floating_point_v<F>, bool> Unpacker::unpack(F & f, int res)
{
	int intFloat;
	if (!unpack(intFloat))
		return false;
	f = static_cast<float>(intFloat) / res;
	return true;
}

template <typename E>
std::enable_if_t<std::is_enum_v<E>, bool> Unpacker::unpack(E & e)
{
	std::underlying_type_t<E> intEnum;
	if (!unpack(intEnum))
		return false;
	e = static_cast<E>(intEnum);
	return true;
}

template <typename T>
std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool> Unpacker::unpack(T & data)
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
std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, bool> Unpacker::unpack(T & data)
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

