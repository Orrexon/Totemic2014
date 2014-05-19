#include "Coinbird.h"
#include "Math.h"
#include "Config.h"
#include "GameStateAssets.h"

Coinbird::Coinbird()
{
	m_animator = new thor::Animator<sf::Sprite, std::string>();
	m_sprite = new sf::Sprite();
}

Coinbird::~Coinbird()
{
	delete m_animator;
	m_animator = nullptr;

	delete m_sprite;
	m_sprite = nullptr;
}

bool Coinbird::isThere()
{
	if (Math::euclideanDistance(m_sprite->getPosition(), m_dropTarget) < COINBIRD_SPEED)
	{
		m_stateAsset->audioSystem->playSound("Coinbird");
		return true;
	}
	return false;
}

bool Coinbird::outofBounds()
{
	int width = 1920;
	int height = 1080;
	int margin = 300;
	int sprite_pos_x = m_sprite->getPosition().x;
	int sprite_pos_y = m_sprite->getPosition().y;

	bool outofX = false;
	bool outofY = false;
	
	if (sprite_pos_x < 0 - margin || sprite_pos_x > width + margin) outofX = true;
	if (sprite_pos_y < 0 - margin || sprite_pos_y > height + margin) outofY = true;

	if (outofX && outofY)
	{
		return true;
	}
	return false;
}
