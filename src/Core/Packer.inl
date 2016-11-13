template <std::int8_t min, std::int8_t max>
void Packer::pack(std::int8_t data)
{
	assert(min < max && "min must be less than max");
	assert(data >= min && "value must be greater than or equal to min");
	assert(data <= max && "value must be less than or equal to max");
	const int bits = BitsRequired<min, max>::result;
	uint8_t unsignedValue = data - min;
	pack8(unsignedValue, bits);
}

template <std::uint8_t min, std::uint8_t max>
void Packer::pack(std::uint8_t data)
{
	assert(min < max && "min must be less than max");
	assert(data >= min && "value must be greater than or equal to min");
	assert(data <= max && "value must be less than or equal to max");
	const int bits = BitsRequired<min, max>::result;
	pack32(data, bits);
}

template <std::int16_t min, std::int16_t max>
void Packer::pack(std::int16_t data)
{
	assert(min < max && "min must be less than max");
	assert(data >= min && "value must be greater than or equal to min");
	assert(data <= max && "value must be less than or equal to max");
	const int bits = BitsRequired<min, max>::result;
	uint16_t unsignedValue = data - min;
	pack16(unsignedValue, bits);
}

template <std::uint16_t min, std::uint16_t max>
void Packer::pack(std::uint16_t data)
{
	assert(min < max && "min must be less than max");
	assert(data >= min && "value must be greater than or equal to min");
	assert(data <= max && "value must be less than or equal to max");
	const int bits = BitsRequired<min, max>::result;
	pack16(data, bits);
}

template <int32_t min, int32_t max>
void Packer::pack(std::int32_t data)
{
	assert(min < max && "min must be less than max");
	assert(data >= min && "value must be greater than or equal to min");
	assert(data <= max && "value must be less than or equal to max");
	const int bits = BitsRequired<min, max>::result;
	uint32_t unsignedValue = data - min;
	pack32(unsignedValue, bits);
}

template <uint32_t min, uint32_t max>
void Packer::pack(std::uint32_t data)
{
	assert(min < max && "min must be less than max");
	assert(data >= min && "value must be greater than or equal to min");
	assert(data <= max && "value must be less than or equal to max");
	const int bits = BitsRequired<min, max>::result;
	pack32(data, bits);
}

template<int minInt, int maxInt, int res>
void Packer::pack(float data)
{
	const int32_t min = minInt * Power<10, res>::value;
	const int32_t max = maxInt * Power<10, res>::value;
	int32_t intValue = (int32_t)(data * Power<10, res>::value);
	pack<min, max>(intValue);
}

template <std::int8_t min, std::int8_t max>
void Unpacker::unpack(std::int8_t & data)
{
	assert(min < max && "min must be less than max");
	const int bits = BitsRequired<min, max>::result;
	uint8_t unsignedValue;
	unpack8(unsignedValue, bits);
	int8_t value = (int8_t)unsignedValue + min;
	data = value;
}

template <std::uint8_t min, std::uint8_t max>
void Unpacker::unpack(std::uint8_t & data)
{
	assert(min < max && "min must be less than max");
	const int bits = BitsRequired<min, max>::result;
	unpack8(data, bits);
}

template <std::int16_t min, std::int16_t max>
void Unpacker::unpack(std::int16_t & data)
{
	assert(min < max && "min must be less than max");
	const int bits = BitsRequired<min, max>::result;
	uint16_t unsignedValue;
	unpack16(unsignedValue, bits);
	int16_t value = (int16_t)unsignedValue + min;
	data = value;
}

template <std::uint16_t min, std::uint16_t max>
void Unpacker::unpack(std::uint16_t & data)
{
	assert(min < max && "min must be less than max");
	const int bits = BitsRequired<min, max>::result;
	unpack16(data, bits);
}

template <int32_t min, int32_t max>
void Unpacker::unpack(std::int32_t & data)
{
	assert(min < max && "min must be less than max");
	const int bits = BitsRequired<min, max>::result;
	uint32_t unsignedValue;
	unpack32(unsignedValue, bits);
	int32_t value = (int32_t)unsignedValue + min;
	data = value;
}

template <uint32_t min, uint32_t max>
void Unpacker::unpack(std::uint32_t & data)
{
	assert(min < max && "min must be less than max");
	const int bits = BitsRequired<min, max>::result;
	unpack32(data, bits);
}

template<int minInt, int maxInt, int res>
void Unpacker::unpack(float & data)
{
	const int32_t min = minInt * Power<10, res>::value;
	const int32_t max = maxInt * Power<10, res>::value;
	int32_t intVal;
	unpack<min, max>(intVal);
	data = (float)intVal / Power<10, res>::value;
}