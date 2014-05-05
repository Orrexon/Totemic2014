#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\Shape.hpp>

namespace Math
{
	bool pointInCircle(sf::Vector2f point, sf::Vector2f circle_center, float circle_radius);

	float euclideanDistance(sf::Vector2f v1, sf::Vector2f v2);

	float angleBetween(const sf::Vector2f &v1, const sf::Vector2f &v2);
}