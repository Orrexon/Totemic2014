#include "Level.h"
#include <iostream>
#include <Box2D\Box2D.h>
#include "PhysicsHelper.h"
#include "ResourceHolder.h"

Level::Level()
{
	m_background = new sf::Sprite();
	m_defenderSpawn.resize(4);
	m_gathererSpawn.resize(4);
}

Level::~Level()
{
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*m_background);
	for (auto &obj : m_objects)
	{
		target.draw(obj->sprite);
	}
}

void Level::addObject(LevelObject* obj)
{
	m_objects.push_back(obj);
}

void Level::setBackgroundPath(std::string p_filepath)
{
	m_backgroundPath = p_filepath;
}
std::string Level::getBackgroundPath()
{
	return m_backgroundPath;
}
void Level::setHotspotPosition(float x, float y)
{
	m_hotspotPosition.x = x;
	m_hotspotPosition.y = y;
}
void Level::setHotspotRadius(float radius)
{
	m_hotspotRadius = radius;
}

void Level::setDefenderSpawn(int player_index, float x, float y)
{
	sf::Vector2f spawn(x, y);
	m_defenderSpawn[player_index] = spawn;
}
void Level::setGathererSpawn(int player_index, float x, float y)
{
	sf::Vector2f spawn(x, y);
	m_gathererSpawn[player_index] = spawn;
}
sf::Vector2f Level::getDefenderSpawn(int player_index)
{
	return m_defenderSpawn[player_index];
}
sf::Vector2f Level::getGathererSpawn(int player_index)
{
	return m_gathererSpawn[player_index];
}
sf::Sprite* Level::getBackground()
{
	return m_background;
}
sf::Vector2f Level::getHotspotPosition()
{
	return m_hotspotPosition;
}
float Level::getHotspotRadius()
{
	return m_hotspotRadius;
}
void Level::constructObjects(b2World* world, ResourceHolder* resourceHolder)
{
	for (auto &object : m_objects)
	{
		std::cout << object->image_path << std::endl;
		// Set texture of sprite
		object->sprite.setTexture(resourceHolder->getTexture(object->image_path, false));

		// Set origin of sprite to center
		//object->sprite.setOrigin(object->sprite.getLocalBounds().width / 2.f, object->sprite.getLocalBounds().height / 2.f);
		
		// Define a static body with position 0, 0
		b2BodyDef bodydef;
		bodydef.type = b2_staticBody;
		bodydef.position = PhysicsHelper::gameToPhysicsUnits(sf::Vector2f(0, 0));
		bodydef.angle = 0;
		object->body = world->CreateBody(&bodydef);

		if (object->use_circle)
		{
			object->body->SetTransform(PhysicsHelper::gameToPhysicsUnits(object->sprite.getPosition()), 0);
			// Create a circle shape
			b2CircleShape circleShape;
			circleShape.m_radius = PhysicsHelper::gameToPhysicsUnits(object->circle_radius);

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.restitution = 0;
			fixtureDef.friction = 1;
			fixtureDef.shape = &circleShape;

			object->body->CreateFixture(&fixtureDef);
		}
		else
		{
			// Create shapes and fixtures for all triangles
			for (auto &triangle : object->triangles)
			{
				b2PolygonShape polygonShape;
			
				b2Vec2 vertices[3];
				vertices[0] = (PhysicsHelper::gameToPhysicsUnits(object->sprite.getTransform() * triangle[0]));
				vertices[1] = (PhysicsHelper::gameToPhysicsUnits(object->sprite.getTransform() * triangle[1]));
				vertices[2] = (PhysicsHelper::gameToPhysicsUnits(object->sprite.getTransform() * triangle[2]));

				polygonShape.Set(vertices, 3);

				b2FixtureDef fixtureDef;
				fixtureDef.density = 1;
				fixtureDef.restitution = 0;
				fixtureDef.friction = 1;
				fixtureDef.shape = &polygonShape;
			
				object->body->CreateFixture(&fixtureDef);
			}
		}
	}
}