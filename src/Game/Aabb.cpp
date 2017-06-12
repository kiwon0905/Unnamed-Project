#include "Aabb.h"

Aabb::Aabb():
	x(0.f),
	y(0.f),
	w(0.f),
	h(0.f)
{
}

Aabb::Aabb(float x, float y, float w, float h):
	x(x),
	y(y),
	w(w),
	h(h)
{
}

Aabb::Aabb(const sf::Vector2f & position, const sf::Vector2f & size):
	x(position.x),
	y(position.y),
	w(size.x),
	h(size.y)
{
}

bool Aabb::contains(float x, float y) const
{
	float minX = (std::min)(x, (x + w));
	float maxX = (std::min)(x, (x + w));
	float minY = (std::min)(y, (y + h));
	float maxY = (std::min)(y, (y + h));

	return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
}

bool Aabb::contains(const sf::Vector2f & point) const
{
	return contains(point.x, point.y);
}

bool Aabb::intersects(const Aabb & aabb) const
{
	Aabb intersection;
	return intersects(aabb, intersection);
}

bool Aabb::intersects(const Aabb & aabb, Aabb & intersection) const
{
	float r1MinX = (std::min)(x, (x + w));
	float r1MaxX = (std::max)(x, (x + w));
	float r1MinY = (std::min)(y, (y + h));
	float r1MaxY = (std::max)(y, (y + h));

	float r2MinX = (std::min)(aabb.x, (aabb.x + aabb.w));
	float r2MaxX = (std::max)(aabb.x, (aabb.x + aabb.w));
	float r2MinY = (std::min)(aabb.y, (aabb.y + aabb.h));
	float r2MaxY = (std::max)(aabb.y, (aabb.y + aabb.h));

	float interLeft = (std::max)(r1MinX, r2MinX);
	float interTop = (std::max)(r1MinY, r2MinY);
	float interRight = (std::min)(r1MaxX, r2MaxX);
	float interBottom = (std::min)(r1MaxY, r2MaxY);

	if ((interLeft < interRight) && (interTop < interBottom))
	{
		intersection = Aabb(interLeft, interTop, interRight - interLeft, interBottom - interTop);
		return true;
	}
	else
	{
		intersection = Aabb();
		return false;
	}
}