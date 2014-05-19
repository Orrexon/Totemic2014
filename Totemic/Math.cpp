#include "Math.h"
#include <cmath>

namespace Math
{
	bool pointInCircle(sf::Vector2f point, sf::Vector2f circle_center, float circle_radius)
	{
		return (euclideanDistance(point, circle_center) <= circle_radius);
	}

	float euclideanDistance(sf::Vector2f v1, sf::Vector2f v2)
	{
		return std::sqrt(std::pow(v1.x - v2.x, 2) + std::pow(v1.y - v2.y, 2));
	}

	float vectorLength(sf::Vector2f v)
	{
		return std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
	}

	float angleBetween(const sf::Vector2f &v1, const sf::Vector2f &v2)
	{
		return std::atan2f(v2.y - v1.y, v2.x - v1.x);
	}

	float roundByMultiple(float n, float multiple)
	{
		return multiple * (std::roundf(n / multiple));
	}

	float clamp(float n, float max, float min)
	{
		if (n > max) return max;
		if (n < min) return min;
		return n;
	}
	
	b2Vec2 collisionPoint(b2Vec2 firstV, b2Vec2 scndV, float Radius)
	{
		b2Vec2 DirV = firstV - scndV;
		float FDirV = DirV.Length();
		DirV = b2Vec2(DirV.x / FDirV, DirV.y / FDirV);
		return Radius * DirV;
	}

	b2Vec2 collisionDirection(b2Vec2 firstV, b2Vec2 scndV)
	{
		b2Vec2 DirV = firstV - scndV;
		float FDirV = DirV.Length();
		DirV = b2Vec2(DirV.x / FDirV, DirV.y / FDirV);
		return DirV;
	}
	sf::Vector2f direction(sf::Vector2f v1, sf::Vector2f v2)
	{
		sf::Vector2f delta = v2 - v1;
		float length = vectorLength(delta);
		return sf::Vector2f(delta.x / length, delta.y / length);
	}

	float RAD2DEG(float rad)
	{
		return rad * 180 / b2_pi;
	}

	float DEG2RAD(float def)
	{
		return def * b2_pi / 180;
	}
}

