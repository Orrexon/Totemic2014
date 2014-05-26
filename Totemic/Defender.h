#pragma once

#include "Player.h"

class b2Body;
class PlayerUD;

class Defender : public PlayerCharBase
{
public:
	Defender();
	~Defender();

	sf::Sprite* m_stunBirds;
	thor::Animator<sf::Sprite, std::string>* m_stunBirdsAnimator;
	thor::FrameAnimation* m_anim;
};

