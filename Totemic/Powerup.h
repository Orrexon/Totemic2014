#pragma once

#include <SFML\Graphics\Sprite.hpp>
#include <string>
#include <Thor\Animation\Animator.hpp>
#include <vector>

namespace thor
{
	class FrameAnimation;
}

enum PowerupEnum
{
	SHIELD,
	LIGHTNING,
	POWERUP_COUNT
};

class Player;

class Powerup
{
public:
	Powerup();
	virtual ~Powerup();

	sf::Sprite* getSprite();
	thor::Animator<sf::Sprite, std::string>* getAnimator();
	thor::FrameAnimation* getAnimation();
	void setType(PowerupEnum type);
	bool isType(PowerupEnum type);
	void setActive(bool value);
	bool isActive();

	PowerupEnum getType();


public:
	int m_coinSpawnIndex;

private:
	bool m_active;
	PowerupEnum m_type;
	sf::Sprite* m_sprite;
	thor::Animator<sf::Sprite, std::string>* m_animatior;
	thor::FrameAnimation* m_animation;
};