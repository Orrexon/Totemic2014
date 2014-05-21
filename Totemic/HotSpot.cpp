#include "HotSpot.h"
#include "Math.h"
#include "Player.h"
#include "Gatherer.h"

#include <SFML\Graphics\CircleShape.hpp>

HotSpot::HotSpot()
{
	mIdleColor.r = 176;
	mIdleColor.g = 207;
	mIdleColor.b = 124;
	mIdleColor.a = 37;
	m_shape = new sf::CircleShape();
	m_shape->setFillColor(mIdleColor);

	m_sprite = new sf::Sprite();
	m_animator = new thor::Animator<sf::Sprite, std::string>();
	m_animation.addFrame(1.f, sf::IntRect(0, 0, 448, 448));
	m_animation.addFrame(1.f, sf::IntRect(448, 0, 448, 448));
	m_animation.addFrame(1.f, sf::IntRect(896, 0, 448, 448));
	m_animation.addFrame(1.f, sf::IntRect(1344, 0, 448, 448));
	m_animation.addFrame(1.f, sf::IntRect(1792, 0, 448, 448));
	m_animator->addAnimation("idle", m_animation, sf::seconds(1.f));
	m_animator->playAnimation("idle", true);
	mIdleColor.a = 200;
	//m_sprite->setColor(mIdleColor);
}

HotSpot::~HotSpot()
{
	delete m_shape;
	m_shape = nullptr;

	delete m_sprite;
	m_sprite = nullptr;

	delete m_animator;
	m_animator = nullptr;
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
		if (player == nullptr) continue;
		if (Math::pointInCircle(player->getGatherer()->getSprite()->getPosition(), m_position, m_radius))
		{
			newVector.push_back(player);
		}
	}
	return newVector;
}


sf::Sprite* HotSpot::getSprite()
{
	return m_sprite;
}

thor::Animator<sf::Sprite, std::string>* HotSpot::getAnimator()
{
	return m_animator;
}