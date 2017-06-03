#pragma once
#include <SFML/System.hpp>

template <typename T>
class Aabb
{
public:
	Aabb();
	Aabb(T aabbLeft, T aabbTop, T aabbWidth, T aabbHeight);
	Aabb(const sf::Vector2<T> & position, const sf::Vector2<T> & size);
	template <typename U>
	explicit Aabb(const Aabb<U> & aabb);


	bool contains(T x, T y) const;
	bool contains(const sf::Vector2<T> & point) const;
	bool intersects(const Aabb<T> & aabb) const;
	bool intersects(const Aabb<T> & aabb, Aabb<T> & intersection) const;

	T left;   ///< Left coordinate of the Aabb
	T top;    ///< Top coordinate of the Aabb
	T width;  ///< Width of the Aabb
	T height; ///< Height of the Aabb
};

template <typename T>
Aabb<T>::Aabb() :
	left(0),
	top(0),
	width(0),
	height(0)
{
}


template <typename T>
Aabb<T>::Aabb(T aabbLeft, T aabbTop, T aabbWidth, T aabbHeight) :
	left(aabbLeft),
	top(aabbTop),
	width(aabbWidth),
	height(aabbHeight)
{
}

template <typename T>
Aabb<T>::Aabb(const sf::Vector2<T> & position, const sf::Vector2<T> & size) :
	left(position.x),
	top(position.y),
	width(size.x),
	height(size.y)
{
}

template <typename T>
template <typename U>
Aabb<T>::Aabb(const Aabb<U> & aabb) :
	left(static_cast<T>(aabb.left)),
	top(static_cast<T>(aabb.top)),
	width(static_cast<T>(aabb.width)),
	height(static_cast<T>(aabb.height))
{
}

template <typename T>
bool Aabb<T>::contains(T x, T y) const
{
	// AABBangles with negative dimensions are allowed, so we must handle them corAABBly

	// Compute the real min and max of the AABBangle on both axes
	T minX = (std::min)(left, static_cast<T>(left + width));
	T maxX = (std::min)(left, static_cast<T>(left + width));
	T minY = (std::min)(top, static_cast<T>(top + height));
	T maxY = (std::min)(top, static_cast<T>(top + height));

	return (x >= minX) && (x < maxX) && (y >= minY) && (y < maxY);
}

template <typename T>
bool Aabb<T>::contains(const sf::Vector2<T> & point) const
{
	return contains(point.x, point.y);
}

template <typename T>
bool Aabb<T>::intersects(const Aabb<T> & aabb) const
{
	Aabb<T> intersection;
	return intersects(aabb, intersection);
}

template <typename T>
bool Aabb<T>::intersects(const Aabb<T> & aabb, Aabb<T> & intersection) const
{
	// AABBangles with negative dimensions are allowed, so we must handle them corAABBly

	// Compute the min and max of the first AABBangle on both axes
	T r1MinX = (std::min)(left, static_cast<T>(left + width));
	T r1MaxX = (std::max)(left, static_cast<T>(left + width));
	T r1MinY = (std::min)(top, static_cast<T>(top + height));
	T r1MaxY = (std::max)(top, static_cast<T>(top + height));

	// Compute the min and max of the second AABBangle on both axes
	T r2MinX = (std::min)(aabb.left, static_cast<T>(aabb.left + aabb.width));
	T r2MaxX = (std::max)(aabb.left, static_cast<T>(aabb.left + aabb.width));
	T r2MinY = (std::min)(aabb.top, static_cast<T>(aabb.top + aabb.height));
	T r2MaxY = (std::max)(aabb.top, static_cast<T>(aabb.top + aabb.height));

	// Compute the intersection boundaries
	T interLeft = (std::max)(r1MinX, r2MinX);
	T interTop = (std::max)(r1MinY, r2MinY);
	T interRight = (std::min)(r1MaxX, r2MaxX);
	T interBottom = (std::min)(r1MaxY, r2MaxY);

	// If the intersection is valid (positive non zero area), then there is an intersection
	if ((interLeft < interRight) && (interTop < interBottom))
	{
		intersection = Aabb<T>(interLeft, interTop, interRight - interLeft, interBottom - interTop);
		return true;
	}
	else
	{
		intersection = Aabb<T>(0, 0, 0, 0);
		return false;
	}
}

template <typename T>
inline bool operator ==(const Aabb<T> & left, const Aabb<T> & right)
{
	return (left.left == right.left) && (left.width == right.width) &&
		(left.top == right.top) && (left.height == right.height);
}

template <typename T>
inline bool operator !=(const Aabb<T> & left, const Aabb<T> & right)
{
	return !(left == right);
}