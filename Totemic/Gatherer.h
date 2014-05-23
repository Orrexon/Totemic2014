#pragma once

#include "Player.h"

class b2Body;

class Gatherer : public PlayerCharBase
{
public:
	Gatherer();
	~Gatherer();

	sf::Sprite* m_shieldOverlay;
	thor::Animator<sf::Sprite, std::string>* m_shieldOverlayAnimatior;
	thor::FrameAnimation* m_anim;
};

