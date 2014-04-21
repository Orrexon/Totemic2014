#pragma once

#include <SFML\System\Vector2.hpp>
#include <vector>

namespace sf
{
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
private:
	float m_radius;
	sf::Vector2f m_position;
	sf::CircleShape* m_shape; // Just for visual debug information
};

