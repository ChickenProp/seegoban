#include "grid.h"

void Grid::corner(float x, float y) {
	if (defined) return;

	corners.push_back(GridPoint(x,y));
	if (corners.size() >= 4)
		defined = true;
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

	corners[nearest] = v;
}

void Grid::moveNearest(ph::vec2f p) {
	moveNearest(p.x, p.y);
}

void Grid::printCorners(FILE *out) {
	for (int i = 0; i < corners.size(); i++)
		fprintf(out, "%f,%f\n", corners[i].x, corners[i].y);
}

void Grid::render(sf::RenderTarget &tgt) {
// SFML's sf::Shape::Line renders using a GL_QUAD, which means it starts to
// disappear if scaled down too much. Using GL_LINES fixes this.

// It's a pretty bad hack. SFML loads the View matrix whenever it draws
// something, which means that changing the View doesn't affect how raw OpenGL
// calls will draw. But with PreserveOpenGLStates off, it doesn't unload the
// matrix after drawing. So after drawing the image in Board::render, OpenGL is
// in a state where we can draw sanely. (Except there's a texture bound, so I
// have to get rid of that.)

// But when we draw a GridPoint, we switch back to the default View to do the
// drawing. So we have to PreserveOpenGLStates for that, to avoid clobbering the
// matrix.

// Another potential problem: I make no attempt to set the target to be active,
// because for some reason only RenderWindows (not generic RenderTargets) have
// the SetActive method.

	tgt.PreserveOpenGLStates(true);

	for (int i = 0; i < corners.size(); i++) {
		corners[i].render(tgt);
	}

	tgt.PreserveOpenGLStates(false);

	if (corners.size() < 4)
		return;

	sf::Shape line;

	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINES);
	glColor4f(1, 0, 0, 0.5);

	for (int x = 1; x <= size; x++) {
		ph::vec2f p1 = getIntersection(x, 1);
		ph::vec2f p2 = getIntersection(x, size);

		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
	}

	for (int y = 1; y <= size; y++) {
		ph::vec2f p1 = getIntersection(1, y);
		ph::vec2f p2 = getIntersection(size, y);

		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
	}

	glEnd();
}

ph::vec2f Grid::getIntersection(int x, int y) {
	int numParallels = 0;

	if ((corners[0] - corners[1]).cross(corners[2] - corners[3]) == 0)
		numParallels++;

	if ((corners[0] - corners[3]).cross(corners[1] - corners[2]) == 0)
		numParallels++;

	switch (numParallels) {
	case 0: return getIntersection2pp(x, y);
	case 1: return getIntersection1pp(x, y);
	}

	// In this case, it's linear.
	
	// Take coordinates 1-indexed, for consistency with Go numbering.
	x -= 1; y -= 1;

	ph::vec2f u = corners[3] - corners[0];
	ph::vec2f v = corners[1] - corners[0];

	return corners[0] + x*u/(size-1) + y*v/(size-1);
}

ph::vec2f Grid::getIntersection1pp (int x, int y) {
	// Take coordinates 1-indexed, for consistency with Go numbering.
	x -= 1; y -= 1;

	ph::vec2f c0, c1, c2, c3;
	ph::vec2f vanish, eye, horizontal;
	ph::vec2f xproj, yproj, tmpproj;

	// Choose c0-c1 to be parallel to c2-c3, even if that's not how the
	// corners themselves are aligned. If that's the case, we have to swap
	// the x and y axes.
	if ((corners[0] - corners[1]).cross(corners[2] - corners[3]) == 0) {
		c0 = corners[0]; c1 = corners[1];
		c2 = corners[2]; c3 = corners[3];
	}
	else {
		c0 = corners[0]; c1 = corners[3];
		c2 = corners[2]; c3 = corners[1];
		int temp = x; x = y; y = temp;
	}

	horizontal =  c1 - c0;
	vanish = ph::vec2f::intersect(c0, c3, c1, c2);
	eye = ph::vec2f::intersect(c1, c3, vanish, vanish + horizontal);

	xproj = c0 + x*horizontal/(size-1);
	tmpproj = c0 + y*horizontal/(size-1);
	yproj = ph::vec2f::intersect(tmpproj, eye, c0, vanish);

	return ph::vec2f::intersect(xproj, vanish, yproj, yproj + horizontal);
}


// See http://void.printf.net/~mad/Perspective/
ph::vec2f Grid::getIntersection2pp(int x, int y) {
	// Take coordinates 1-indexed, for consistency with Go numbering.
	x -= 1; y -= 1;

	ph::vec2f c0, c1, c2, c3;
	ph::vec2f horiz1, horiz2, proj1, proj2, parallel;
	ph::vec2f xproj, yproj;

	c0 = corners[0]; c1 = corners[1]; c2 = corners[2]; c3 = corners[3];

	horiz1 = ph::vec2f::intersect(c0, c1, c2, c3);
	horiz2 = ph::vec2f::intersect(c0, c3, c1, c2);

	parallel = c0 + horiz1 - horiz2;
	proj1 = ph::vec2f::intersect(horiz1, c2, c0, parallel);
	proj2 = ph::vec2f::intersect(horiz2, c2, c0, parallel);

	xproj = c0 + x*(proj1 - c0)/(size-1);
	yproj = c0 + y*(proj2 - c0)/(size-1);

	return ph::vec2f::intersect(xproj, horiz1, yproj, horiz2);
}
