#include "HotSpot.h"
#include "Math.h"
#include "Player.h"
#include "Gatherer.h"

#include <SFML\Graphics\CircleShape.hpp>

HotSpot::HotSpot()
{
	m_shape = new sf::CircleShape();
	m_shape->setFillColor(sf::Color(100, 255, 100, 100));
}

HotSpot::~HotSpot()
{
	delete m_shape;
	m_shape = nullptr;
}

void HotSpot::setRadius(float radius)
{
	m_radius = radius;
	m_shape->setRadius(radius);
	m_shape->setOrigin(radius, radius);
}
void HotSpot::setPosition(sf::Vector2f position)
{
	m_position = position;
	m_shape->setPosition(position);
}
float HotSpot::getRadius()
{
	return m_radius;
}
sf::Vector2f HotSpot::getPosition()
{
	return m_position;
}
sf::CircleShape* HotSpot::getShape()
{
	return m_shape;
}
std::vector<Player*> HotSpot::getActivePlayers(std::vector<Player*> &players)
{
	std::vector<Player*> newVector;
	for (auto &player : players)
	{
		if (Math::pointInCircle(player->getGatherer()->getSprite()->getPosition(), m_position, m_radius))
		{
			newVector.push_back(player);
		}
	}
	return newVector;
}