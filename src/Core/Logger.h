#pragma once
#include <iostream>
#include <SFML/System.hpp>

#include "Aabb.h"
#include <mutex>
class Logger
{
public:
	void info(const std::string & s);
	void debug(const std::string & s);
	void warn(const std::string & s);
	void error(const std::string & s);

	Logger(const Logger &) = delete;
	Logger & operator=(const Logger &) = delete;

	static Logger & getInstance()
	{
		static Logger instance;
		return instance;
	}
private:
	Logger() {}
	std::mutex m_mutex;
};


template <typename T>
std::ostream & operator<<(std::ostream & os, const sf::Vector2<T> & v)
{
	return os << "( " << v.x << ", " << v.y << " )";
}

template <typename T>
std::ostream & operator<<(std::ostream & os, const Aabb<T> & aabb)
{
	return os << "( " << aabb.left << ", " << aabb.top << ", " << aabb.left + aabb.width << ", " << aabb.top + aabb.height << " )";
}
