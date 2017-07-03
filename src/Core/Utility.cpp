#include "Utility.h"
#include <ctime>
static std::default_random_engine generator(time(nullptr));

int randInt(int a, int b)
{
	std::uniform_int_distribution<int> distribution(a, b);
	return distribution(generator);
}

float randFloat(float a, float b)
{
	std::uniform_real_distribution<float> distribution(a, b);
	return distribution(generator);
}