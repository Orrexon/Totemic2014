#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\Shape.hpp>
#include <algorithm>
#include <Box2D\Box2D.h>

namespace Math
{
	bool pointInCircle(sf::Vector2f point, sf::Vector2f circle_center, float circle_radius);

	float euclideanDistance(sf::Vector2f v1, sf::Vector2f v2);

	float vectorLength(sf::Vector2f v);

	float angleBetween(const sf::Vector2f &v1, const sf::Vector2f &v2);

	float roundByMultiple(float n, float multiple);

	float clamp(float n, float max, float min);
	
	b2Vec2 collisionPoint(b2Vec2 firstV, b2Vec2 scndV, float Radius);
	
	b2Vec2 collisionDirection(b2Vec2 firstV, b2Vec2 scndV);

	sf::Vector2f direction(sf::Vector2f v1, sf::Vector2f v2);

	float sumMagnitude(sf::Vector2f& V1, sf::Vector2f& V2);//overloaded for flexibility

	float sumMagnitude(const b2Vec2& V1, const b2Vec2& V2);

	float sumMagnitude(b2Vec2& V1, sf::Vector2f& V2);

	float sumMagnitude(sf::Vector2f& V1, b2Vec2& V2);

	float RAD2DEG(float rad);
	float DEG2RAD(float def);
}

