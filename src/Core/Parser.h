#pragma once

#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>
#include <sstream>
#include <istream>

class Parser
{
public:
	bool loadFromFile(const std::string & file);
	bool save(const std::string & file = "");
	void reset();
	void print(std::ostream & os = std::cout);
	void clear();
	template <class T>
	bool get(const std::string & key, T & value);
	template <class T>
	bool get(const std::string & key, std::vector<T> & values);
	template <class T>
	void set(const std::string & key, const T & value);
	template <class T>
	void set(const std::string & key, const std::vector<T> & values);
	template <class T>
	void set(const std::string & key, const std::initializer_list<T> & values);
private:
	std::size_t find(const std::string & key);

	template <class T>
	T castTo(const std::string & s);
	template <class T>
	std::string toString(T val);
	std::string Parser::trim(const std::string & s);
private:
	std::string m_fileName;
	std::vector<std::pair<std::string, std::string>> m_data;
};

template <class T>
bool Parser::get(const std::string & key, T & value)
{
	std::size_t i = find(key);
	if (i == -1)
		return false;
	try
	{
		value = castTo<T>(m_data[i].second);
	}
	catch (const std::exception &)
	{
		return false;
	}
	return true;
}

template <class T>
bool Parser::get(const std::string & key, std::vector<T> & values)
{
	std::size_t i = find(key);
	if (i == -1)
		return false;

	std::vector<T> retrievedValues;
	std::string token;
	std::istringstream ss(m_data[i].second);
	try
	{
		while (std::getline(ss, token, ','))
			retrievedValues.emplace_back(castTo<T>(token));
	}
	catch (const std::exception &)
	{
		return false;
	}
	values = retrievedValues;

	return true;
}

template <class T>
void Parser::set(const std::string & key, const T & value)
{
	std::size_t i = find(key);
	if (i == -1)
		m_data.emplace_back(key, toString(value));
	else
		m_data[i].second = toString(value);
}

template <class T>
void Parser::set(const std::string & key, const std::vector<T> & values)
{
	if (values.size() == 0)
		return;

	std::string value;
	for (const T & val : values)
		value += (toString(val) + ", ");
	
	value.erase(value.end() - 2, value.end());

	std::size_t i = find(key);
	if (i == -1)
		m_data.emplace_back(key, value);
	else
		m_data[i].second = value;
}

template<class T>
inline void Parser::set(const std::string & key, const std::initializer_list<T> & values)
{
	set(key, std::vector<T>(values));
}

template <>
inline std::string Parser::castTo<std::string>(const std::string & s)
{
	return s;
}

template <>
inline bool Parser::castTo<bool>(const std::string & s)
{
	bool b;
	std::istringstream iss(s);
	iss >> std::boolalpha >> b;
	if (iss.fail())
		throw std::exception();
	return b;
}

template <class T>
T Parser::castTo(const std::string & s)
{
	T var;
	std::istringstream iss(s);
	iss >> var;
	if (iss.fail())
		throw std::exception();

	return var;
}

template <>
inline std::string Parser::toString(bool val)
{
	if (val)
		return std::string("true");
	return std::string("false");
}

template <>
inline std::string Parser::toString(const char * val)
{
	return trim(std::string(val));
}

template <>
inline std::string Parser::toString(std::string val)
{
	return trim(val);
}

template <class T>
std::string Parser::toString(T val)
{
	return std::to_string(val);
}