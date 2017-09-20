#include "Logger.h"
#include <string>

void Logger::info(const std::string & tag, const std::string & s)
{
	m_mutex.lock();
	std::cout << "[INFO]" << "[" << tag << "] " << s << "\n";
	m_mutex.unlock();
}

void Logger::debug(const std::string & tag, const std::string & s)
{
	m_mutex.lock();
	std::cout << "[DEBUG]" << "[" << tag << "] " << s << "\n";
	m_mutex.unlock();
}

void Logger::warn(const std::string & tag, const std::string & s)
{
	m_mutex.lock();
	std::cout << "[WARN]" << "[" << tag << "] " << s << "\n";
	m_mutex.unlock();
}

void Logger::error(const std::string & tag, const std::string & s)
{
	m_mutex.lock();
	std::cout << "[ERROR]" << "[" << tag << "] " << s << "\n";
	m_mutex.unlock();
}