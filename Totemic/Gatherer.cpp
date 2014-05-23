#include "Gatherer.h"
#include <SFML\Graphics\Sprite.hpp>
#include <Thor\Animation.hpp>

Gatherer::Gatherer()
{
	m_shieldOverlayAnimatior = new thor::Animator<sf::Sprite, std::string>;
	m_shieldOverlay = new sf::Sprite();

	thor::FrameAnimation* shield_overlay_animation = new thor::FrameAnimation();
	shield_overlay_animation->addFrame(1.f, sf::IntRect(0, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(146, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(292, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(438, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(584, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(730, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(876, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(1022, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(1168, 0, 146, 138));
	shield_overlay_animation->addFrame(1.f, sf::IntRect(1314, 0, 146, 138));
	m_anim = shield_overlay_animation;
	m_shieldOverlayAnimatior->addAnimation("Idle", *shield_overlay_animation, sf::seconds(0.35f));
	m_shieldOverlayAnimatior->playAnimation("Idle", true);
}

Gatherer::~Gatherer()
{
	delete m_shieldOverlayAnimatior;
	m_shieldOverlayAnimatior = nullptr;

	delete m_shieldOverlay;
	m_shieldOverlay = nullptr;

	delete m_anim;
	m_anim = nullptr;
}