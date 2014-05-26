#include "Defender.h"
#include "Config.h"
#include <SFML\Graphics\Sprite.hpp>

Defender::Defender()
{
	m_stunBirdsAnimator = new thor::Animator<sf::Sprite, std::string>;
	m_stunBirds = new sf::Sprite();

	thor::FrameAnimation* stunBirdsAnimation = new thor::FrameAnimation();
	int size = 256;
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 0, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 1 + 1, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 2 + 2, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 3 + 3, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 4 + 4, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 5 + 5, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 6 + 6, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 7 + 7, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 8 + 8, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 9 + 9, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 10 + 10, 0, size, size));
	stunBirdsAnimation->addFrame(1.f, sf::IntRect(size * 11 + 11, 0, size, size));
	m_anim = stunBirdsAnimation;

	m_stunBirdsAnimator->addAnimation("Idle", *stunBirdsAnimation, sf::seconds(DEFENDER_STUN_BIRDS_TIME));
	m_stunBirdsAnimator->playAnimation("Idle", true);
}

Defender::~Defender()
{
	delete m_stunBirdsAnimator;
	m_stunBirdsAnimator = nullptr;

	delete m_anim;
	m_anim = nullptr;

	delete m_stunBirds;
	m_stunBirds = nullptr;
}