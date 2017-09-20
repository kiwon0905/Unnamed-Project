#pragma once
#include <iostream>
#include <mutex>
class Logger
{
public:

	void info(const std::string & tag, const std::string & s);
	void debug(const std::string & tag, const std::string & s);
	void warn(const std::string & tag, const std::string & s);
	void error(const std::string & tag, const std::string & s);
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