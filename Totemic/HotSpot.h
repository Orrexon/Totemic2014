#pragma once

#include <SFML\System\Vector2.hpp>
#include <vector>
#include <Thor\Animation\Animator.hpp>
#include <Thor\Animation\FrameAnimation.hpp>
#include <SFML\Graphics\Color.hpp>

namespace sf
{
	class Sprite;
	class CircleShape;
}

class Player;

class HotSpot
{
public:
	HotSpot();
	~HotSpot();

	void setRadius(float radius);
	void setPosition(sf::Vector2f position);
	std::vector<Player*> getActivePlayers(std::vector<Player*> &players);

	float getRadius();
	sf::Vector2f getPosition();
	sf::CircleShape* getShape();

	sf::Sprite* getSprite();
	thor::Animator<sf::Sprite, std::string>* getAnimator();
private:
	float m_radius;
	sf::Vector2f m_position;
	sf::Sprite* m_sprite;
	sf::Color mIdleColor;
	thor::Animator<sf::Sprite, std::string>* m_animator;
	thor::FrameAnimation m_animation;
	sf::CircleShape* m_shape; // Just for visual debug information
};

