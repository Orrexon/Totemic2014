#pragma once

#include <string>

#include "Level.h"

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	Level* parseLevel(const std::string &filepath);
	void setDirectory(const std::string &filepath);

	LevelObject* createObject(std::string filepath, sf::Vector2f position);

private:
	std::string m_levelDirectory;
};

