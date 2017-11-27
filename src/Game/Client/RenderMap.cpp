#include "RenderMap.h"

bool RenderMap::loadFromTmx(const std::string & s)
{
	using namespace tinyxml2;
	m_document.LoadFile(s.c_str());

	XMLElement * map = m_document.FirstChildElement("map");



	XMLElement * collisionLayer = getElementWithAttribute(map, "layer", "name", "Collision Layer");
	return true;
}
