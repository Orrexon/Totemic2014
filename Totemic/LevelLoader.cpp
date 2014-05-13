#include <fstream>
#include <json\json.h>
#include <Thor\Math.hpp>
#include <Box2D\Collision\Shapes\b2CircleShape.h>
#include <Box2D\Collision\Shapes\b2PolygonShape.h>
#include <Box2D\Dynamics\b2Fixture.h>

#include "Level.h"
#include "LevelLoader.h"
#include "String.h"
#include "ResourceHolder.h"
#include "Trap.h"
#include "PhysicsHelper.h"

LevelLoader::LevelLoader()
{
}
LevelLoader::~LevelLoader()
{
}

Level* LevelLoader::parseLevel(const std::string &filepath, b2World& world)
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
		level->setPlayerSpawn(it.index(), 
			sf::Vector2f((*it)["defender_position_x"].asDouble(), (*it)["defender_position_y"].asDouble()),
			sf::Vector2f((*it)["gatherer_position_x"].asDouble(), (*it)["gatherer_position_y"].asDouble()));
		level->setDefenderSpawn(it.index(), (*it)["defender_position_x"].asDouble(), (*it)["defender_position_y"].asDouble());
		level->setGathererSpawn(it.index(), (*it)["gatherer_position_x"].asDouble(), (*it)["gatherer_position_y"].asDouble());
	}

	// Create objects from parsed file
	for (auto it = root["objects"].begin(); it != root["objects"].end(); ++it)
	{
		LevelObject* obj = createObject((*it)["path"].asString(), sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()), world);
		level->addObject(obj);
	}

	// Create traps
	for (auto it = root["traps"].begin(); it != root["traps"].end(); ++it)
	{
		Trap* trap = createTrap(sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
		level->addTrap(trap);
	}

	// Create coin spawns
	for (auto it = root["coins"].begin(); it != root["coins"].end(); ++it)
	{
		level->addCoinSpawn(sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
	}

	// Create powerup spawns
	for (auto it = root["powerups"].begin(); it != root["powerups"].end(); ++it)
	{
		level->addPowerupSpawn(sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
	}

	// Create image objects
	for (auto it = root["image_objects"].begin(); it != root["image_objects"].end(); ++it)
	{
		Image* image = new Image();
		image->sprite.setPosition(sf::Vector2f((*it)["x"].asDouble(), (*it)["y"].asDouble()));
		image->sprite.setTexture(m_resourceHolder->getTexture((*it)["image_path"].asString(), false));
		image->sprite.setOrigin(image->sprite.getGlobalBounds().width / 2.f, image->sprite.getGlobalBounds().height / 2.f);
		level->addImage(image);
	}
	return level;
}

void LevelLoader::setDirectory(const std::string &filepath)
{
	m_levelDirectory = filepath;
}

void LevelLoader::setResourceHolder(ResourceHolder* resourceHolder)
{
	m_resourceHolder = resourceHolder;
}

LevelObject* LevelLoader::createObject(std::string filepath, sf::Vector2f position, b2World& world)
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

	// Set texture of sprite
	levelObj->sprite.setTexture(m_resourceHolder->getTexture(levelObj->image_path, false));

	// Define a static body with position 0, 0
	b2BodyDef bodydef;
	bodydef.type = b2_staticBody;
	bodydef.position = PhysicsHelper::gameToPhysicsUnits(sf::Vector2f(0, 0));
	bodydef.angle = 0;
	levelObj->body = world.CreateBody(&bodydef);

	if (levelObj->use_circle)
	{
		levelObj->body->SetTransform(PhysicsHelper::gameToPhysicsUnits(levelObj->sprite.getPosition()), 0);
		// Create a circle shape
		b2CircleShape circleShape;
		circleShape.m_radius = PhysicsHelper::gameToPhysicsUnits(levelObj->circle_radius);

		b2FixtureDef fixtureDef;
		fixtureDef.density = 1;
		fixtureDef.restitution = 0;
		fixtureDef.friction = 1;
		fixtureDef.shape = &circleShape;

		levelObj->body->CreateFixture(&fixtureDef);
	}
	else
	{
		// Create shapes and fixtures for all triangles
		for (auto &triangle : levelObj->triangles)
		{
			b2PolygonShape polygonShape;

			b2Vec2 vertices[3];
			vertices[0] = (PhysicsHelper::gameToPhysicsUnits(levelObj->sprite.getTransform() * triangle[0]));
			vertices[1] = (PhysicsHelper::gameToPhysicsUnits(levelObj->sprite.getTransform() * triangle[1]));
			vertices[2] = (PhysicsHelper::gameToPhysicsUnits(levelObj->sprite.getTransform() * triangle[2]));

			polygonShape.Set(vertices, 3);

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.restitution = 0;
			fixtureDef.friction = 1;
			fixtureDef.shape = &polygonShape;

			levelObj->body->CreateFixture(&fixtureDef);
		}
	}
	return levelObj;
}

Trap* LevelLoader::createTrap(sf::Vector2f position)
{
	Trap* trap = new Trap();
	trap->getSprite().setPosition(position.x, position.y - 93);
	trap->getSprite().setOrigin(128, 128);
	trap->getSprite().setTexture(m_resourceHolder->getTexture("trap.png"));
	trap->setExplosionPosition(position.x, position.y);
	trap->setExplosionRadius(125);
	return trap;
}