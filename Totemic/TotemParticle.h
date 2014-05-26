#pragma once

#include <Thor\Animation\FrameAnimation.hpp>
#include <Thor\Animation\Animator.hpp>
#include <SFML\Graphics\Sprite.hpp>

class TotemParticle
{
public:
	TotemParticle();
	~TotemParticle();

	sf::Sprite *m_sprite;
	thor::Animator<sf::Sprite, std::string> *m_animator;
	thor::FrameAnimation m_animation;
};

