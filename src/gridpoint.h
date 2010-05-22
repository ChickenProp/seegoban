#ifndef SGB_GRIDPOINT_H
#define SGB_GRIDPOINT_H

#include <SFML/Graphics.hpp>
#include "vector.h"

class GridPoint : public ph::vec2f {
public:
	bool fixed;

	GridPoint(float x, float y)
		: ph::vec2f(x,y), fixed(false) {}
	GridPoint(ph::vec2f v)
		: ph::vec2f(v), fixed(false) {}

	void render(sf::RenderTarget &target);
};
	

#endif
