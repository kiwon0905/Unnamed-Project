#include "Logger.h"
#include <string>
void Logger::info(const std::string & s)
{
	m_mutex.lock();
	std::cout << "[INFO]: " << s << "\n";
	m_mutex.unlock();
}

void Logger::debug(const std::string & s)
{
#ifdef _DEBUG
	m_mutex.lock();
	std::cout << "[DEBUG]: " << s << "\n";
	m_mutex.unlock();
#endif
}

void Logger::warn(const std::string & s)
{
	m_mutex.lock();
	std::cout << "[WARN]: " << s << "\n";
	m_mutex.unlock();
}

void Logger::error(const std::string & s)
{
	m_mutex.lock();
	std::cout << "[ERROR]: " << s << "\n";
	m_mutex.unlock();
}
