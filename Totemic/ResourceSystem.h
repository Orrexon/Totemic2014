#pragma once

#include <Thor\Resources.hpp>

class ResourceSystem
{
public:
	ResourceSystem();
	~ResourceSystem();

	template<typename T>
	std::shared_ptr<T>
private:
	thor::MultiResourceCache cache;
};

