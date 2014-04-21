#include <fstream>
#include <json\json.h>
#include <Thor\Math.hpp>

#include "Level.h"
#include "LevelLoader.h"
#include "String.h"

LevelLoader::LevelLoader()
{
}
LevelLoader::~LevelLoader()
{
}

Level* LevelLoader::parseLevel(const std::string &filepath)
{
	std::ifstream istream;
	istream.open(m_levelDirectory + filepath, std::ifstream::binary);
	if (!istream.is_open())
	{
		std::cout << "Failed to load level: Can't open file" << std::endl;
		return nullptr;
	}

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(istream, root, false))
	{
		std::cout << "Failed to parse level " << m_levelDirectory + filepath << " with message: " << reader.getFormatedErrorMessages() << std::endl;
		return nullptr;
	}

	Level* level = new Level();
	level->setHotspotPosition(root["hotspot"]["position"]["x"].asDouble(), root["hotspot"]["position"]["y"].asDouble());
	level->setHotspotRadius(root["hotspot"]["radius"].asDouble());
	level->setBackgroundPath(root["background"]["path"].asString());
	for (auto it = root["players"].begin(); it != root["players"].end(); ++it)
	{
		level->setDefenderSpawn(it.index(), (*it)["defender_position_x"].asDouble(), (*it)["defender_position_y"].asDouble());
		level->setGathererSpawn(it.index(), (*it)["gatherer_position_x"].asDouble(), (*it)["gatherer_position_y"].asDouble());
	}

	// Create objects from parsed file
	for (auto it = root["objects"].begin(); it != root["objects"].end(); ++it)
	{
		LevelObject* obj = createObject((*it)["path"].asString(), sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
		
		level->addObject(obj);
	}
	return level;
}

void LevelLoader::setDirectory(const std::string &filepath)
{
	m_levelDirectory = filepath;
}

LevelObject* LevelLoader::createObject(std::string filepath, sf::Vector2f position)
{
	std::ifstream istream;
	istream.open(m_levelDirectory + filepath, std::ifstream::binary);
	LevelObject* levelObj = new LevelObject();
	levelObj->sprite.setPosition(position);
	if (istream.is_open())
	{
		Json::Value rt;
		Json::Reader reader;
		if (reader.parse(istream, rt, false))
		{
			
			// If we don't use circle collision
			if (!rt["use_circle"].asBool())
			{
				levelObj->use_circle = false;
				// fetch vertices
				levelObj->vertices.clear();
				for (auto it = rt["points"].begin(); it != rt["points"].end(); ++it)
				{
					levelObj->vertices.push_back(sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
				}

				// remove last vertex
				levelObj->vertices.pop_back();

				// triangulate
				levelObj->triangles.clear();
				thor::triangulatePolygon(levelObj->vertices.begin(), levelObj->vertices.end(), std::back_inserter(levelObj->triangles));
			}
			else
			{
				levelObj->use_circle = true;
				levelObj->circle_radius = rt["circle"]["radius"].asDouble();
			}
			
			levelObj->image_path = rt["image"]["path"].asString();
		}
	}
	return levelObj;
}