#include "Aabb.h"
#include "Core/Utility.h"
#include <iostream>

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

bool Aabb::broadphase(const sf::Vector2f & v, const Aabb & aabb) const
{
	Aabb area;
	area.x = v.x > 0 ? x : x + v.x;
	area.y = v.y > 0 ? y : y + v.y;
	area.w = v.x > 0 ? v.x + w : w - v.x;
	area.h = v.y > 0 ? v.y + h : h - v.y;

	return aabb.intersects(area);
}

bool Aabb::sweep(const sf::Vector2f & displacement, const Aabb & aabb, float & time, sf::Vector2i & norm) const
{
	if (!broadphase(displacement, aabb))
	{
		norm.x = 0;
		norm.y = 0;
		time = 1.f;
		return false;
	}
	sf::Vector2f entryDistance, exitDistance;

	if (displacement.x > 0.f)
	{
		entryDistance.x = aabb.x - (x + w);
		exitDistance.x = (aabb.x + aabb.w) - x;
		norm.x = -1;
	}
	else
	{
		entryDistance.x = (aabb.x + aabb.w) - x;
		exitDistance.x = aabb.x - (x + w);
		norm.x = 1;
	}
	if (displacement.y > 0.f)
	{
		entryDistance.y = aabb.y - (y + h);
		exitDistance.y = (aabb.y + aabb.h) - y;
		norm.y = -1;
	}
	else
	{
		entryDistance.y = (aabb.y + aabb.h) - y;
		exitDistance.y = aabb.y - (y + h);
		norm.y = 1;
	}

	sf::Vector2f entryTime, exitTime;

	if (displacement.x == 0.f)
	{
		entryTime.x = -std::numeric_limits<float>::infinity();
		exitTime.x = std::numeric_limits<float>::infinity();
	}
	else
	{
		entryTime.x = entryDistance.x / displacement.x;
		exitTime.x = exitDistance.x / displacement.x;
	}
	if (displacement.y == 0.f)
	{
		entryTime.y = -std::numeric_limits<float>::infinity();
		exitTime.y = std::numeric_limits<float>::infinity();
	}
	else
	{
		entryTime.y = entryDistance.y / displacement.y;
		exitTime.y = exitDistance.y / displacement.y;
	}

	float maxEntryTime = std::max(entryTime.x, entryTime.y);
	float minExitTime = std::min(exitTime.x, exitTime.y);

	//no collision
	if (maxEntryTime > minExitTime || entryTime.x < 0.f && entryTime.y < 0.f || entryTime.x > 1.f || entryTime.y > 1.f)
	{
		norm.x = 0;
		norm.y = 0;
		time = 1.f;
		return false;
	}
	else
	{
		if (entryTime.x < entryTime.y)
		{
			norm.x = 0;

		}
		else //if(entryTime.x > entryTime.y)
		{
			norm.y = 0;

		}
		time = maxEntryTime;

		return true;
	}

}


bool lineLine(const sf::Vector2f & a, const sf::Vector2f & b,
	const sf::Vector2f & c, const sf::Vector2f & d,
	sf::Vector2f & intersection)
{
	float uA = ((d.x - c.x)*(a.y - c.y) - (d.y - c.y)*(a.x - c.x)) / ((d.y - c.y)*(b.x - a.x) - (d.x - c.x)*(b.y - a.y));
	float uB = ((b.x - a.x)*(a.y - c.y) - (b.y - a.y)*(a.x - c.x)) / ((d.y - c.y)*(b.x - a.x) - (d.x - c.x)*(b.y - a.y));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
	{
		intersection.x = a.x + (uA * (b.x - a.x));
		intersection.y = a.y + (uA * (b.y - a.y));

		return true;
	}
	return false;
}

bool Aabb::testLine(const sf::Vector2f & p, const sf::Vector2f & q, float & t)
{
	sf::Vector2f minIntersection;
	float minDistance = std::numeric_limits<float>::max();

	bool collided = false;

	float minX = (std::min)(x, (x + w));
	float maxX = (std::max)(x, (x + w));
	float minY = (std::min)(y, (y + h));
	float maxY = (std::max)(y, (y + h));


	//left
	sf::Vector2f intersection;
	if (lineLine(p, q, { minX, minY }, { minX, maxY }, intersection))
	{
		float d = Math::length(p - intersection);
		if (d < minDistance)
		{
			minDistance = d;
			minIntersection = intersection;
		}
		collided = true;
	}
	//right
	if (lineLine(p, q, { maxX, minY }, { maxX, maxY }, intersection))
	{
		float d = Math::length(p - intersection);
		if (d < minDistance)
		{
			minDistance = d;
			minIntersection = intersection;
		}
		collided = true;		

	}
	//up
	if (lineLine(p, q, { minX, minY }, { maxX, minY }, intersection))
	{
		float d = Math::length(p - intersection);
		if (d < minDistance)
		{
			minDistance = d;
			minIntersection = intersection;
		}
		collided = true;

	}
	//down
	if (lineLine(p, q, { minX, maxY }, { maxX, maxY }, intersection))
	{
		float d = Math::length(p - intersection);
		if (d < minDistance)
		{
		
			minDistance = d;
			minIntersection = intersection;
		}
		collided = true;

	}
	if (collided)
	{
		t = minDistance / Math::length(q - p);
	}
	

	return collided;
}

bool Aabb::sweepPoints(const std::vector<sf::Vector2f>& points, const sf::Vector2f & d, float & t)
{
	bool collided = false;

	float minTime = std::numeric_limits<float>::max();

	for (const auto & p : points)
	{
		float temp;
		if (testLine(p, p + d, temp))
		{
			collided = true;
			if (temp < minTime)
				minTime = temp;
		}
	}
	if (collided)
	{
		t = minTime;
	}
	return collided;
}
