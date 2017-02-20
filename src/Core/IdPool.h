#pragma once

#include <vector>

template <typename T>
class IdPool
{
public:
	void checkIn(T id);
	T checkOut();
private:
	std::vector<T> m_pool;
	T m_nextId = 0;
};

template <typename T>
void IdPool<T>::checkIn(T id)
{
	m_pool.push_back(id);
}

template <typename T>
T IdPool<T>::checkOut()
{
	if (!m_pool.empty())
	{
		T id = m_pool.back();
		m_pool.pop_back();
		return id;
	}
	return m_nextId++;
}
