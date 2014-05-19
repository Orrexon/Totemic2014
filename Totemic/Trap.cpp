#include "Trap.h"
#include "Config.h"

Trap::Trap()
{
	m_active = false;
	m_exploding = false;
	m_crackle.addFrame(1.f, sf::IntRect(0,    0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(257,  0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(514,  0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(771,  0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(1028, 0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(1285, 0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(1542, 0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(1799, 0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(2056, 0, 256, 256));
	m_crackle.addFrame(1.f, sf::IntRect(2313, 0, 256, 256));

	m_explosion.addFrame(1.f, sf::IntRect(2570, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(2827, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(3084, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(3341, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(3598, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(3855, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(4112, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(4369, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(4626, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(4883, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(5140, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(5397, 0, 256, 256));
	m_explosion.addFrame(1.f, sf::IntRect(5654, 0, 256, 256));

	m_animator.addAnimation("crackle", m_crackle, sf::seconds(TRAP_LOADS_SECONDS));
	m_animator.addAnimation("explosion", m_explosion, sf::seconds(TRAP_EXPLODES_SECONDS));
}

Trap::~Trap()
{

}

sf::Sprite& Trap::getSprite()
{
	return m_sprite;
}

void Trap::setExplosionPosition(float x, float y)
{
	m_explosionPosition.x = x;
	m_explosionPosition.y = y;
}

void Trap::setExplosionRadius(float radius)
{
	m_explosionRadius = radius;
}

void Trap::setActive(bool value)
{
	m_active = value;
}

void Trap::setExploding(bool value)
{
	m_exploding = value;
}

thor::Animator<sf::Sprite, std::string> &Trap::getAnimator()
{
	return m_animator;
}

float Trap::getExplosionRadius()
{
	return m_explosionRadius;
}

sf::Vector2f Trap::getExplosionPosition()
{
	return m_explosionPosition;
}

bool Trap::isActive()
{
	return m_active;
}

bool Trap::isExploding()
{
	return m_exploding;
}