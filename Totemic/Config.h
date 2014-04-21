#pragma once

#include <string>

class Config
{
public:
	Config();
	~Config();

	void setRoot(std::string path);

	std::string getRoot();

private:
	std::string m_rootPath;
};

