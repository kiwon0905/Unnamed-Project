#pragma once
#include <SFML/System.hpp>

class Aabb
{
public:
	Aabb();
	Aabb(float aabbLeft, float aabbTop, float aabbWidth, float aabbHeight);
	Aabb(const sf::Vector2f & position, const sf::Vector2f & size);


	bool contains(float x, float y) const;
	bool contains(const sf::Vector2f & point) const;
	bool intersects(const Aabb & aabb) const;
	bool intersects(const Aabb & aabb, Aabb & intersection) const;

	bool sweep(const sf::Vector2f & velocity, const Aabb & aabb, float & time, sf::Vector2i & norm) const;
	float x;   ///< Left coordinate of the Aabb
	float y;    ///< Top coordinate of the Aabb
	float w;  ///< Width of the Aabb
	float h; ///< Height of the Aabb

private:
	bool broadphase(const sf::Vector2f & velocity, const Aabb & aabb) const;
};