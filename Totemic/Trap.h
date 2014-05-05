#pragma once

#include <SFML\Graphics\Sprite.hpp>
#include <SFML\System\Vector2.hpp>
#include <Thor\Animation\Animator.hpp>
#include <Thor\Animation\FrameAnimation.hpp>

class Trap
{
public:
	Trap();
	~Trap();

	sf::Sprite& getSprite();
	void setExplosionPosition(float x, float y);
	void setExplosionRadius(float radius);
	void setActive(bool value);
	void setExploding(bool value);

	thor::Animator<sf::Sprite, std::string> &getAnimator();
	float getExplosionRadius();
	sf::Vector2f getExplosionPosition();
	bool isActive();
	bool isExploding();
private:
	bool m_active;
	bool m_exploding;
	sf::Vector2f m_explosionPosition;
	float m_explosionRadius;
	thor::Animator<sf::Sprite, std::string> m_animator;
	thor::FrameAnimation m_crackle;
	thor::FrameAnimation m_explosion;
	sf::Sprite m_sprite;
};