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

	/* We want to calculate t in [0, 1], the value such that moving 1/t
	 * along one side and 1/t along the opposite side gives two points which
	 * are colinear with each other and the centre point of the board.
	 *
	 * Points x, y, z are colinear if (x-y) and (z-y) are parallel, ie. if
	 * (x-y) x (z-y) = 0. So letting the corners be c0 through c3 and the
	 * centre be r, we need t such that

	 * (c0 + t(c1-c0) - r) x (c3 + t(c2-c3) - r) = 0

	 * Make the following substitutions: */

	ph::vec2f p1, p2, p3, p4;
	p1 = corners[1] - corners[0];
	p2 = corners[0] - centre;
	p3 = corners[2] - corners[3];
	p4 = corners[3] - centre;

	/* Expanding the brackets, we now have:

	 * t^2 (p1 x p3) + t(p1 x p4 + p2 x p3) + (p2 x p4) = 0

	 * which is easy to solve for t. One solution is the value we desire;
	 * the other will give us the point where the sides intersect. If the
	 * sides are parallel, we get a=0 and must instead solve a linear
	 * equation with only one solution (but solveQuadratic handles this
	 * case, so it doesn't really matter). */

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

	/* t is for the left and right sides. Now we do the same to find s, the
	 * equivalent point for the top and bottom sides. */

	p1 = corners[3] - corners[0];
	p2 = corners[0] - centre;
	p3 = corners[2] - corners[1];
	p4 = corners[1] - centre;

	a = p1.cross(p3);
	b = p1.cross(p4) + p2.cross(p3);
	c = p2.cross(p4);

	float s, s1, s2;
	solveQuadratic(a, b, c, &s1, &s2);

	if (0 <= s1 && s1 <= 1)
		s = s1;
	else if (0 <= s2 && s2 <= 1)
		s = s2;
	else
		s = 0.5;


	GridPoint sidel((1-t)*corners[0] + t*corners[1]);
	GridPoint sider((1-t)*corners[3] + t*corners[2]);
	GridPoint sidet((1-s)*corners[0] + s*corners[3]);
	GridPoint sideb((1-s)*corners[1] + s*corners[2]);

	sidel.render(tgt);
	sider.render(tgt);
	sidet.render(tgt);
	sideb.render(tgt);

	//vec2f r(s,t); /* The centre coordinate in gridspace. */

	renderSemi(tgt, corners[0], sidel, corners[3], sider, (size+1)/2.0);
	renderSemi(tgt, corners[1], sidel, corners[2], sider, (size+1)/2.0);
	renderSemi(tgt, corners[0], sidet, corners[1], sideb, (size+1)/2.0);
	renderSemi(tgt, corners[3], sidet, corners[2], sideb, (size+1)/2.0);
}

void Grid::renderSemi(sf::RenderTarget &tgt, ph::vec2f s11, ph::vec2f s12,
                      ph::vec2f s21, ph::vec2f s22, float numlines)
{
	sf::Shape line;
	ph::vec2f u = (s12 - s11)/(numlines - 1);
	ph::vec2f v = (s22 - s21)/(numlines - 1);

	for (int i = 0; i < (int) numlines; i++) {
		line = sf::Shape::Line(s11 + i*u, s21 + i*v,
		                       1, sf::Color(255,0,0, 128));

		tgt.Draw(line);
	}
}

/* Find roots of a quadratic equation ax^2 + bx + c = 0. Returns the number of
 * real roots. Also handles linear (a=0) and constant (a=b=0) cases. If a=b=c=0
 * there are infinitely many roots, but 3 is returned.

 * If the fourth argument is not NULL, it will be set to the first root.

 * If the fifth argument is not NULL, it will be set to the second root, which
 * may be the same as the first.

 * If there are no roots, or a=b=c=0, x1 and x2 are unchanged. */
int solveQuadratic(float a, float b, float c, float *x1, float *x2)
{
	if (a == 0 && b == 0)
		return (c == 0 ? 3 : 0);
	else if (a == 0) {
		if (x1) *x1 = -c/b;
		if (x2) *x2 = -c/b;
		return 1;
	}

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
