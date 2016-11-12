#include "Parser.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>

bool Parser::loadFromFile(const std::string & file)
{
	reset();
	std::ifstream fin(file);
	if (!fin.is_open())
		return false;

	std::string line;
	while (std::getline(fin, line))
	{
		//empty line
		if (!line.size())
			m_data.emplace_back("", "");
		//comment
		else if (line[0] == '#')
			m_data.emplace_back(line, "");
		else
		{
			std::size_t pos = line.find('=');
			if (pos == std::string::npos)
			{
				reset();
				return false;
			}
			std::string key = trim(line.substr(0, pos));
			std::string value = trim(line.substr(pos + 1));
			if (key == "" || value == "")
			{
				reset();
				return false;
			}
			m_data.emplace_back(key, value);
		}
	}

	m_fileName = file;
	return true;
}

bool Parser::save(const std::string & file)
{
	std::ofstream fout;
	if (file == "")
		fout.open(m_fileName);
	else
		fout.open(file);
	
	if (!fout.is_open())
		return false;

	print(fout);
	return true;
}

void Parser::reset()
{
	m_fileName = "";
	m_data.clear();
}

void Parser::print(std::ostream & os)
{
	for (const auto & pair : m_data)
	{
		if (pair.first != "")
		{
			if (pair.first[0] != '#')
				os << pair.first << " = " << pair.second;
			else
				os << pair.first;
		}
		os << "\n";
	}
}

void Parser::clear()
{
	m_data.clear();
}

std::size_t Parser::find(const std::string & key)
{
	for (std::size_t i = 0; i < m_data.size(); ++i)
		if (m_data[i].first == key)
			return i;

	return -1;
}

std::string Parser::trim(const std::string & s)
{
	std::string str = s;
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	str.erase(std::find_if(str.rbegin(), str.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
	return str;
}
