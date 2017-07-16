#pragma once

#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <random>
#include <functional>

template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Vector2<T> & v)
{
	return os << "(" << v.x << ", " << v.y << ")";
}

namespace Math
{
	const float PI = 3.1415926535f;

	template <typename T>
	T length(const sf::Vector2<T> & v)
	{
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	template <typename T>
	sf::Vector2<T> unit(const sf::Vector2<T> & v)
	{
		return v / length(v);
	}

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
		return static_cast<T>((1 - t)*a + t*b);
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

	template<typename T>
	inline T clampedAdd(T min, T max, T current, T modifier)
	{
		T val = current + modifier;
		if (val < min)
			val = min;
		if (val > max)
			val = max;
		return val;
	}

	inline int roundToInt(float f)
	{
		if (f < 0)
			return static_cast<int>(f - .5f);
		return static_cast<int>(f + .5f);
	}

	inline float normalizedAngle(float x) {
		x = fmod(x, 360);
		if (x < 0)
			x += 360;
		return x;
	}

	namespace
	{
		extern std::default_random_engine generator(time(nullptr));
	}

	template<typename T>
	typename std::enable_if_t<std::is_integral<T>::value, T> rand(T min, T max)
	{
		return std::uniform_int_distribution<T>(min, max)(generator);
	}

	template<typename T>
	typename std::enable_if_t<std::is_floating_point<T>::value, T> rand(T min, T max)
	{
		return std::uniform_real_distribution<T>(min, max)(generator);

	}

	template<typename T>
	struct DistributionFunc
	{
		DistributionFunc(T constant)
		{
			m_function = [constant]()
			{
				return constant;
			};
		}

		DistributionFunc(std::function<T()> fn)
		{
			m_function = fn;
		}

		T operator()() const
		{
			return m_function();
		}
	private:
		std::function<T()> m_function;
	};


	DistributionFunc<int> uniform(int min, int max);
	DistributionFunc<float> uniform(float min, float max);
	DistributionFunc<sf::Vector2f> circle(const sf::Vector2f & center, float radius);
	
}


