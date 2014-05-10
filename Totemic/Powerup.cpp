#include "Powerup.h"
#include "Thor\Animation\FrameAnimation.hpp"


Powerup::Powerup()
{
	m_sprite = new sf::Sprite();
	m_animatior = new thor::Animator<sf::Sprite, std::string>;
	m_animation = new thor::FrameAnimation();
	m_active = true;
}

Powerup::~Powerup()
{
}

sf::Sprite* Powerup::getSprite()
{
	return m_sprite;
}
thor::Animator<sf::Sprite, std::string> *Powerup::getAnimator()
{
	return m_animatior;
}
thor::FrameAnimation* Powerup::getAnimation()
{
	return m_animation;
}

void Powerup::setType(PowerupEnum type)
{
	m_type = type;
}

PowerupEnum Powerup::getType()
{
	return m_type;
}

bool Powerup::isType(PowerupEnum type)
{
	if (type == m_type) return true;
	return false;
}

void Powerup::setActive(bool value)
{
	m_active = value;
}
bool Powerup::isActive()
{
	return m_active;
}
