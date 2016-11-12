#include "ResourceManager.h"

void ResourceManager::release(const std::string & id)
{
	m_resources.erase(id);
}

ResourceManager::ErasedPtr::~ErasedPtr()
{
	deleter();
}