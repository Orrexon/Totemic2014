#pragma once

#include <SFML\Graphics\Sprite.hpp>
#include <Thor\Animation.hpp>

class Coin;
struct StateAsset;
class Coinbird
{
public:
	Coinbird();
	~Coinbird();

	bool isThere();
	bool outofBounds();

public:
	StateAsset *m_stateAsset;
	sf::Vector2f m_dropTarget;
	sf::Vector2f m_velocity;
	float m_speed;
	Coin* m_coin;
	sf::Sprite *m_sprite;
	thor::Animator<sf::Sprite, std::string>* m_animator;
	thor::FrameAnimation m_animation;
};

