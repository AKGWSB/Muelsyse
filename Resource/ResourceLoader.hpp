#pragma once

#include <memory>
#include <string>
#include <unordered_map>

template<typename T>
class ResourceLoader
{
private:
	ResourceLoader() {};
	ResourceLoader(const ResourceLoader&);
	ResourceLoader& operator=(const ResourceLoader&);

	std::unordered_map<std::string, std::unique_ptr<T>> m_resourceMap;

public:
	~ResourceLoader() {};

	// single ton
	static ResourceLoader<T>* GetInstance()
	{
		static ResourceLoader<T> instance;
		return &instance;
	}

	// find resource by "reference", using filepath as ref
	T* Find(std::string filepath)
	{
		auto it = m_resourceMap.find(filepath);
		if (it == m_resourceMap.end())
		{
			// resource type T must support "LoadFromFile"
			m_resourceMap[filepath] = std::make_unique<T>(filepath);
			return m_resourceMap[filepath].get();
		}
		return (*it).second.get();
	}

	void Shutdown()
	{
		for (auto& up : m_resourceMap)
		{
			delete up.second.release();
		}
	}
};


