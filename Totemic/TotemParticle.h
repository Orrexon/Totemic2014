#pragma once

#include <SFML\Graphics\Sprite.hpp>
#include <Thor\Time\Timer.hpp>

class TotemParticle
{
public:
	TotemParticle();
	~TotemParticle();

	sf::Sprite *m_sprite;
	float fadeInAlpha;
	thor::Timer deathTimer;
	bool borned;
};

