#include "grid.h"

void Grid::corner(float x, float y) {
	if (defined) return;

	corners.push_back(GridPoint(x,y));
	if (corners.size() >= 4) {
		defined = true;

		centre = GridPoint( (corners[0] + corners[1]
		                     + corners[2] + corners[3]) / 4);
	}
}

void Grid::moveNearest(float x, float y) {
	ph::vec2f v(x,y);

	int nearest = -1;
	for (int i = 1; i < corners.size(); i++) {
		if (nearest < 0
		    || v.distance(corners[i]) < v.distance(corners[nearest]))
		{
			nearest = i;
		}
	}

	if (!defined || v.distance(centre) >= v.distance(corners[nearest]))
		corners[nearest] = v;
	else
		centre = v;
}

void Grid::render(sf::RenderTarget &tgt) {
	for (int i = 0; i < corners.size(); i++) {
		corners[i].render(tgt);
	}

	if (corners.size() < 4)
		return;

	centre.render(tgt);

	sf::Shape line;

	ph::vec2f u = (corners[1] - corners[0])/(size-1);
	ph::vec2f v = (corners[2] - corners[3])/(size-1);
	for (int i = 0; i < size; i++) {
		line = sf::Shape::Line(corners[0] + i*u,
		                       corners[3] + i*v,
		                       1, sf::Color(255,0,0));

		tgt.Draw(line);
	}

	u = (corners[3] - corners[0])/(size-1.f);
	v = (corners[2] - corners[1])/(size-1.f);
	for (int i = 0; i < size; i++) {
		line = sf::Shape::Line(corners[0] + (float)i*u,
		                       corners[1] + (float)i*v,
		                       1, sf::Color(255,0,0));

		tgt.Draw(line);
	}
}
