#ifndef SGB_GRID_H
#define SGB_GRID_H

#include <SFML/Graphics.hpp>
#include "gridpoint.h"
#include <vector>

class Grid {
public:
	int size;
	std::vector<GridPoint> corners;
	GridPoint centre;
	bool defined;

	Grid() : size(-1), defined(false), centre(0,0) {}

	Grid(int sz) : size(sz), defined(false), centre(0,0) {}

	void corner(float x, float y);
	void corner(ph::vec2f p);
	void moveNearest(float x, float y);
	void moveNearest(ph::vec2f p);
	void render(sf::RenderTarget &target);
	void renderSemi(sf::RenderTarget &tgt, ph::vec2f c1, ph::vec2f c2,
	                ph::vec2f c3, ph::vec2f c4, float numlines);

	ph::vec2f sideMidpoint(int side);
	ph::vec2f transformToLocal(const ph::vec2f &point);
	ph::vec2f getIntersection(int x, int y);
};

#endif
