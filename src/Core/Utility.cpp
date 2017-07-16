#include "Utility.h"

namespace Math
{
	DistributionFunc<int> uniform(int min, int max)
	{
		return [=]()
		{
			return rand(min, max);
		};
	}
	DistributionFunc<float> uniform(float min, float max)
	{
		return [=]()
		{
			return rand(min, max);
		};
	}
	DistributionFunc<sf::Vector2f> circle(const sf::Vector2f & center, float radius)
	{
		return [=]()
		{
			float r = rand(0.f, radius);
			float a = rand(0.f, 2 * Math::PI);

			sf::Vector2f v;
			v.x = r * std::cos(a);
			v.y = r * std::sin(a);
			return v + center;
		};
	}
}
