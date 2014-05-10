#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics\Shape.hpp>
#include <algorithm>
#include <Box2D\Box2D.h>

namespace Math
{
	bool pointInCircle(sf::Vector2f point, sf::Vector2f circle_center, float circle_radius);

	float euclideanDistance(sf::Vector2f v1, sf::Vector2f v2);

	float angleBetween(const sf::Vector2f &v1, const sf::Vector2f &v2);

	float roundByMultiple(float n, float multiple);

	float clamp(float n, float max, float min);
	
	b2Vec2 collisionPoint(b2Vec2 firstV, b2Vec2 scndV, float Radius);
	
	b2Vec2 collisionDirection(b2Vec2 firstV, b2Vec2 scndV);
}

//may not be used
struct VCollection
{
public:
	b2Vec2 PhysDefender_CollisionP = { 0.f, 0.f };
	b2Vec2 physDefender_CollisionD = { 0.f, 0.f };

	
};