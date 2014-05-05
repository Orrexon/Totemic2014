#pragma once

#include <Thor\Animation\FrameAnimation.hpp>
#include <Thor\Animation\Animator.hpp>

namespace sf
{
	class Sprite;
}

class Coin
{
public:
	Coin();
	~Coin();

	sf::Sprite *getSprite();
	thor::Animator<sf::Sprite, std::string> *getAnimator();
	thor::FrameAnimation &getAnimation();

public:
	int m_coinSpawnIndex;

private:
	sf::Sprite* m_sprite;
	thor::Animator<sf::Sprite, std::string>* m_animator;
	thor::FrameAnimation m_animation;
};

