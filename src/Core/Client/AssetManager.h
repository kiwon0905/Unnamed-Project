#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

class AssetManager
{
public:
	template <class T>
	T * get(const std::string & id);
	void release(const std::string & id);
private:
	struct ErasedPtr
	{
		template <class T>
		ErasedPtr(T * t);
		~ErasedPtr();
		ErasedPtr(const ErasedPtr &) = delete;
		ErasedPtr & operator=(const ErasedPtr &) = delete;

		void * data;
		std::function<void()> deleter;
	};

	std::unordered_map<std::string, ErasedPtr> m_resources;
};

template <typename T>
T * AssetManager::get(const std::string & id)
{
	//if resource has never been loaded
	if (m_resources.count(id) == 0)
	{
		T * resource = new T;
		if (!resource->loadFromFile(id))
		{
			delete resource;
			return nullptr;
		}
		m_resources.emplace(id, resource);
	}
	return static_cast<T*>(m_resources.at(id).data);
}

template <typename T>
AssetManager::ErasedPtr::ErasedPtr(T * t)
{
	data = t;
	deleter = [this]()
	{
		delete static_cast<T*>(data);
	};
}


