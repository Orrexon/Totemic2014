#include "Coin.h"
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\Rect.hpp>
#include "Player.h"


Coin::Coin()
{
	m_gathered = false;
	m_sprite = new sf::Sprite();
	m_animator = new thor::Animator<sf::Sprite, std::string>;
	m_animation.addFrame(1.f, sf::IntRect(0, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(65, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(130, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(195, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(260, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(325, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(390, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(455, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(520, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(585, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(650, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(715, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(780, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(845, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(975, 0, 64, 64));
	m_animator->addAnimation("idle", m_animation, sf::seconds(1.f));
}

Coin::~Coin()
{
	delete m_sprite;
	m_sprite = nullptr;

	delete m_animator;
	m_animator = nullptr;
}

sf::Sprite *Coin::getSprite()
{
	return m_sprite;
}

thor::Animator<sf::Sprite, std::string> *Coin::getAnimator()
{
	return m_animator;
}

thor::FrameAnimation &Coin::getAnimation()
{
	return m_animation;
}

void Coin::setGathered(Player* playerGathered)
{
	m_playerGathered = playerGathered;
	m_gathered = true;
}

bool Coin::isGathered()
{
	return m_gathered;
}
Player* Coin::getPlayerGathered()
{
	return m_playerGathered;
}
