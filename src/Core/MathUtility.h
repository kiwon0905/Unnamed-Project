#pragma once

template <typename T>
T clamp(T value, T low, T high)
{
	if (value < low)
		return value;
	if (value > high)
		return high;
	return value;
}

template <typename T>
T lerp(T a, T b, float t)
{
	return (1 - t)*a + t*b;
}

template <typename T>
int comp(T a, T b, T threshold)
{
	if (std::abs(a - b) < threshold)
		return 0;
	if (a > b)
		return -1;
	return 1;
}