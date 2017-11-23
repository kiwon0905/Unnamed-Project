#include "Utility.h"


std::string getStringFromTime(int totalSeconds)
{
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	std::string minutesString = std::to_string(minutes);
	std::string secondsString = std::to_string(seconds);
	if (minutes < 10)
		minutesString = "0" + minutesString;
	if (seconds < 10)
		secondsString = "0" + secondsString;
	return minutesString + ":" + secondsString;
}

namespace Math
{
	sf::Vector2f rotatePoint(const sf::Vector2f & p, float angle)
	{
		float s = std::sin(angle);
		float c = std::cos(angle);

		sf::Vector2f pr;
		pr.x = p.x * c - p.y * s;
		pr.y = p.x * s + p.y * c;
		return pr;
	}
	sf::Vector2f uniformCircle(const sf::Vector2f & center, float radius)
	{
		float r = uniform(0.f, radius);
		float a = uniform(0.f, 2 * Math::PI);

		sf::Vector2f v;
		v.x = r * std::cos(a);
		v.y = r * std::sin(a);
		return v + center;
	}
}
