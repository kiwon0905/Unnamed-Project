#include "AssetManager.h"

void AssetManager::release(const std::string & id)
{
	m_resources.erase(id);
}

AssetManager::ErasedPtr::~ErasedPtr()
{
	deleter();
}