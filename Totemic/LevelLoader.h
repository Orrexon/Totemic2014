#pragma once

#include <string>
#include <Box2D\Dynamics\b2World.h>

class ResourceHolder;
class Level;
struct LevelObject;

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	Level* parseLevel(const std::string &filepath, b2World& world);
	void setDirectory(const std::string &filepath);
	void setResourceHolder(ResourceHolder* resourceHolder);

	LevelObject* createObject(std::string filepath, sf::Vector2f position, b2World& world);
	Trap* createTrap(sf::Vector2f position);

private:
	std::string m_levelDirectory;
	ResourceHolder* m_resourceHolder;
};

