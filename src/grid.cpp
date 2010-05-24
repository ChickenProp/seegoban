#include "grid.h"

int solveQuadratic(float a, float b, float c,
                   float *x1 = NULL, float *x2 = NULL);

void Grid::corner(float x, float y) {
	if (defined) return;

	corners.push_back(GridPoint(x,y));
	if (corners.size() >= 4) {
		defined = true;

		centre = GridPoint( (corners[0] + corners[1]
		                     + corners[2] + corners[3]) / 4);
	}
}

void Grid::corner(ph::vec2f p) {
	corner(p.x, p.y);
}

void Grid::moveNearest(float x, float y) {
	ph::vec2f v(x,y);

	int nearest = 0;
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

void Grid::moveNearest(ph::vec2f p) {
	moveNearest(p.x, p.y);
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
		                       1, sf::Color(255,0,0, 128));

		tgt.Draw(line);
	}

	u = (corners[3] - corners[0])/(size-1.f);
	v = (corners[2] - corners[1])/(size-1.f);
	for (int i = 0; i < size; i++) {
		line = sf::Shape::Line(corners[0] + (float)i*u,
		                       corners[1] + (float)i*v,
		                       1, sf::Color(255,0,0, 128));

		tgt.Draw(line);
	}

	GridPoint sidel(centre.project(corners[0], corners[1]));
	GridPoint sider(centre.project(corners[3], corners[2]));
	GridPoint sidet(centre.project(corners[0], corners[3]));
	GridPoint sideb(centre.project(corners[2], corners[1]));

	sidel.render(tgt);
	sider.render(tgt);
	sidet.render(tgt);
	sideb.render(tgt);

	ph::vec2f p1, p2, p3, p4;
	p1 = corners[1] - corners[0];
	p2 = corners[0] - centre;
	p3 = corners[2] - corners[3];
	p4 = corners[3] - centre;

	float a = p1.cross(p3);
	float b = p1.cross(p4) + p2.cross(p3);
	float c = p2.cross(p4);

	float t, t1, t2;
	solveQuadratic(a, b, c, &t1, &t2);

	if (0 <= t1 && t1 <= 1)
		t = t1;
	else if (0 <= t2 && t2 <= 1)
		t = t2;
	else
		t = 0.5;

	printf("%f\n", t);
}

/* Find roots of a quadratic equation ax^2 + bx + c = 0. Returns the number of
 * real roots.

 * If the fourth argument is not NULL, it will be set to the first root.

 * If the fifth argument is not NULL, it will be set to the second root, which
 * may be the same as the first.

 * If there are no roots, x1 and x2 are unchanged. */
int solveQuadratic(float a, float b, float c, float *x1, float *x2)
{
	float disc = b*b - 4*a*c;
	if (disc < 0) {
		return 0;
	}
	else if (disc == 0) {
		float rt = -b/(2*a);
		if (x1) *x1 = rt;
		if (x2) *x2 = rt;
		return 1;
	}
	else {
		if (x1) *x1 = (-b + sqrt(disc))/(2*a);
		if (x2) *x2 = (-b - sqrt(disc))/(2*a);
		return 2;
	}
}
