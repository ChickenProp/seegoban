#ifndef SGB_GRID_H
#define SGB_GRID_H

#include <SFML/Graphics.hpp>
#include "gridpoint.h"
#include <vector>

class Grid {
public:
	int size;
	std::vector<GridPoint> corners;
	bool defined;

	Grid() : size(-1), defined(false) {}

	Grid(int sz) : size(sz), defined(false) {}

	void corner(float x, float y);
	void corner(ph::vec2f p);
	void moveNearest(float x, float y);
	void moveNearest(ph::vec2f p);
	void render(sf::RenderTarget &target);

	ph::vec2f getIntersection(int x, int y);
	ph::vec2f getIntersection1pp(int x, int y);
	ph::vec2f getIntersection2pp(int x, int y);
};

#endif
