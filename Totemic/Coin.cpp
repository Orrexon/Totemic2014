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
	m_animation.addFrame(1.f, sf::IntRect(64, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(128, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(192, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(256, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(320, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(384, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(448, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(512, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(576, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(640, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(704, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(768, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(832, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(896, 0, 64, 64));
	m_animation.addFrame(1.f, sf::IntRect(960, 0, 64, 64));
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

void Coin::setState(CoinState state)
{
	m_state = state;
}
bool Coin::isState(CoinState state)
{
	return m_state == state;
}