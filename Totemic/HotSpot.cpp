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
	mIdleColor.a = 50;
	m_shape = new sf::CircleShape();
	m_shape->setFillColor(mIdleColor);
	m_shape->setPointCount(128);

	m_sprite = new sf::Sprite();
	m_animator = new thor::Animator<sf::Sprite, std::string>();

	int size = 448;

	m_blueAnim.addFrame(1.f, sf::IntRect(size * 0, 0, size, size));
	m_blueAnim.addFrame(1.f, sf::IntRect(size * 1, 0, size, size));
	m_blueAnim.addFrame(1.f, sf::IntRect(size * 2, 0, size, size));
	m_blueAnim.addFrame(1.f, sf::IntRect(size * 3, 0, size, size));
	m_blueAnim.addFrame(1.f, sf::IntRect(size * 4, 0, size, size));

	m_idleAnim.addFrame(1.f, sf::IntRect(size * 0, size * 1 + 1, size, size));
	m_idleAnim.addFrame(1.f, sf::IntRect(size * 1, size * 1 + 1, size, size));
	m_idleAnim.addFrame(1.f, sf::IntRect(size * 2, size * 1 + 1, size, size));
	m_idleAnim.addFrame(1.f, sf::IntRect(size * 3, size * 1 + 1, size, size));
	m_idleAnim.addFrame(1.f, sf::IntRect(size * 4, size * 1 + 1, size, size));

	m_purpleAnim.addFrame(1.f, sf::IntRect(size * 0, size * 2 + 1, size, size));
	m_purpleAnim.addFrame(1.f, sf::IntRect(size * 1, size * 2 + 1, size, size));
	m_purpleAnim.addFrame(1.f, sf::IntRect(size * 2, size * 2 + 1, size, size));
	m_purpleAnim.addFrame(1.f, sf::IntRect(size * 3, size * 2 + 1, size, size));
	m_purpleAnim.addFrame(1.f, sf::IntRect(size * 4, size * 2 + 1, size, size));

	m_redAnim.addFrame(1.f, sf::IntRect(size * 0, size * 3 + 1, size, size));
	m_redAnim.addFrame(1.f, sf::IntRect(size * 1, size * 3 + 1, size, size));
	m_redAnim.addFrame(1.f, sf::IntRect(size * 2, size * 3 + 1, size, size));
	m_redAnim.addFrame(1.f, sf::IntRect(size * 3, size * 3 + 1, size, size));
	m_redAnim.addFrame(1.f, sf::IntRect(size * 4, size * 3 + 1, size, size));

	m_yellowAnim.addFrame(1.f, sf::IntRect(size * 0, size * 4 + 1, size, size));
	m_yellowAnim.addFrame(1.f, sf::IntRect(size * 1, size * 4 + 1, size, size));
	m_yellowAnim.addFrame(1.f, sf::IntRect(size * 2, size * 4 + 1, size, size));
	m_yellowAnim.addFrame(1.f, sf::IntRect(size * 3, size * 4 + 1, size, size));
	m_yellowAnim.addFrame(1.f, sf::IntRect(size * 4, size * 4 + 1, size, size));

	m_animator->addAnimation("idle", m_idleAnim, sf::seconds(0.5f));
	m_animator->addAnimation("blue", m_blueAnim, sf::seconds(0.5f));
	m_animator->addAnimation("red", m_redAnim, sf::seconds(0.5f));
	m_animator->addAnimation("yellow", m_yellowAnim, sf::seconds(0.5f));
	m_animator->addAnimation("purple", m_purpleAnim, sf::seconds(0.5f));
	m_animator->playAnimation("idle", true);
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
		if (Math::pointInCircle(player->getGatherer()->getSprite()->getPosition(), m_position, m_radius) && !player->isDead() && !player->isDying())
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