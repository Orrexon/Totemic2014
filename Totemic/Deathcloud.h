#pragma once

#include <SFML\Graphics\Sprite.hpp>
#include <Thor\Time\Timer.hpp>

struct DeathCloud
{
	DeathCloud();
	sf::Sprite sprite;
	float alphaTween;
	thor::Timer timer;
	bool borned;
	bool dead;
	bool isDying;
};

