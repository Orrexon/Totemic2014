#include "Config.h"


Config::Config()
{
}


Config::~Config()
{
}


void Config::setRoot(std::string path)
{
	m_rootPath = path;
}


std::string Config::getRoot()
{
	return m_rootPath;
}