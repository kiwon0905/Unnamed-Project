#pragma once
#include <SFML/System.hpp>
#include <vector>

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

	//move this aabb against the other
	bool sweep(const sf::Vector2f & velocity, const Aabb & aabb, float & time, sf::Vector2i & norm) const;
	bool testLine(const sf::Vector2f & p, const sf::Vector2f & q, float & t);
	bool sweepPoints(const std::vector<sf::Vector2f> & points, const sf::Vector2f & d, float & t);

	float x;
	float y;
	float w;
	float h;

private:
	bool broadphase(const sf::Vector2f & velocity, const Aabb & aabb) const;
};