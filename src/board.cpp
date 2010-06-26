#include "board.h"
#include "ph-utils/math-util.h"

sf::Color colorAverage(std::vector<sf::Color> colors);
float colorBrightness(sf::Color color);
float colorSaturation(sf::Color color);

Board::Board() {}
Board::Board(int size, const sf::Image &img, FILE *expect)
	: grid(size), image(img), sprite(), view(),
	  hasExpect(false), expectedStones()
{
	float w = image.GetWidth()/2;
	float h = image.GetHeight()/2;
	view.SetCenter(w, h);
	view.SetHalfSize(w, h);
	sprite.SetImage(img); // 'image' here doesn't seem to work?

	if (expect) {
		hasExpect = true;
		expectedStones = readExpected(expect);
	}
}

void Board::render (sf::RenderTarget &tgt) {
	tgt.Draw(sprite);
	grid.render(tgt);
}

Stone Board::getStoneAtPoint(ph::vec2f p) {
	ph::vec2f l = sprite.TransformToLocal(p);
	int x = (int) l.x;
	int y = (int) l.y;

	sf::Color avg = colorAverage(getSurroundingPixels(x, y, 6));
	float brt = colorBrightness(avg);
	float sat = colorSaturation(avg);

	Stone c;

	if (sat > 50)
		c = Stone::none(x, y, brt, sat);
	else if (brt < 150)
		c = Stone::black(x, y, brt, sat);
	else
		c = Stone::white(x, y, brt, sat);
		
	return c;
}

Stone Board::getStoneAtIntersection(int x, int y) {
	return getStoneAtPoint(grid.getIntersection(x, y));
}

// If we don't have any expected stones, put an 'x' to make it clear that
// something unexpected has happened.
Stone Board::expectedStone(int x, int y) {
	if (hasExpect)
		return expectedStones[y-1][x-1];
	else {
		Stone s;
		s.color = 'x';
		return s;
	}
}

std::vector<sf::Color> Board::getSurroundingPixels(int x, int y, int size) {
	// This doesn't do what it says it does. (Specifically, it doesn't get
	// the right number of surrounding pixels.) That's okay for now because
	// I use it pretty arbitrarily, so it just needs to do something that
	// looks sort of similar to what it says. I'll fix this when I need to.

	std::vector<sf::Color> pixels;

	for (int i = x - size/2; i < x + size/2; i++) {
		for (int j = y - size/2; j < y + size/2; j++) {
			pixels.push_back(image.GetPixel(i, j));
		}
	}

	return pixels;
}

void Board::openInCgoban () {
	FILE *stream = popen("cgoban -edit /dev/stdin", "w");
	if (stream == NULL) {
		fprintf(stderr, "Could not open cgoban for some reason.\n");
		return;
	}
	else {
		printSgf(stream);
		pclose(stream);
	}
}

void Board::viewGraph () {
	FILE *stream = popen("xgraph -nl -P", "w");
	if (stream == NULL) {
		fprintf(stderr, "Could not open xgraph for some reason.\n");
		return;
	}
	else {
		printXGraph(stream);
		pclose(stream);
	}
}

void Board::printText (FILE *file) {
	for (int i = 1; i <= grid.size; i++) {
		for (int j = 1; j <= grid.size; j++) {
			ph::vec2f pt = grid.getIntersection(j, i);
			fprintf(file, "%c ", getStoneAtPoint(pt).color);
		}
		fprintf(file, "\n");
	}
}

void Board::printStoneSgf(int x, int y, FILE *file) {
	// The format for a stone is AB[xy] or AW[xy] where x and y are single
	// letters a-s representing intersections.

	Stone s = getStoneAtIntersection(x, y);
	char c = s.color;
	if (c == '.')
		return;

	char xcoord = '`' + x; // '`' is the character before 'a'
	char ycoord = '`' + y;
	fprintf(file, "A%c[%c%c]\n", c, xcoord, ycoord);
}

void Board::printSgf (FILE *file) {
	// I don't really understand SGF, this is taken from perl's Image2SGF.
	fprintf(file, "(;GM[1]FF[4]SZ[%d]\n", grid.size);
	// PL[B] means it's black's turn to play, which is an okay default.
	fprintf(file, "GN[seegoban conversion]\n\nAP[seegoban]\nPL[B]\n\n");

	for (int i = 1; i <= grid.size; i++)
		for (int j = 1; j <= grid.size; j++)
			printStoneSgf(j, i, file);

	fprintf(file, ")\n");
	fflush(file);
}

void Board::printStoneDebug (int x, int y, FILE *file) {
	Stone s = getStoneAtIntersection(x, y);

	if (hasExpect)
		fprintf(file, "%d\t%d\t%c\t%d\t%d\t%f\t%f\t%c\n",
		        x, y, s.color, s.x, s.y, s.brightness, s.saturation,
		        expectedStone(x, y).color);
	else
		fprintf(file, "%d\t%d\t%c\t%d\t%d\t%f\t%f\n",
		        x, y, s.color, s.x, s.y, s.brightness, s.saturation);
}

void Board::printDebug (FILE *file) {
	for (int i = 1; i <= grid.size; i++)
		for (int j = 1; j <= grid.size; j++)
			printStoneDebug(j, i, file);
}

void printXGraphSet (std::vector<Stone> stones, const char *name, FILE *file) {
	fprintf(file, "\"%s\"\n", name);

	for (int i = 0; i < stones.size(); i++)
		fprintf(file, "%f %f\n",
		        stones[i].brightness, stones[i].saturation);

	fprintf(file, "\n");
}

void Board::printXGraph (FILE *file) {
	std::vector<Stone> black;
	std::vector<Stone> white;
	std::vector<Stone> none;
	std::vector<Stone> all;

	for (int y = 1; y <= grid.size; y++) {
		for (int x = 1; x <= grid.size; x++) {
			Stone s = getStoneAtIntersection(x, y);
			Stone e;
			if (hasExpect)
				e = expectedStone(x, y);

			(!hasExpect ? all
			 : e.color == 'B' ? black
			 : e.color == 'W' ? white
			 : none).push_back(s);
		}
	}

	if (hasExpect) {
		printXGraphSet(none, "None", file);
		printXGraphSet(black, "Black", file);
		printXGraphSet(white, "White", file);
	}
	else
		printXGraphSet(all, "Stones", file);
}


void Board::printStoneExpected (int x, int y, FILE *file) {
	Stone s = getStoneAtIntersection(x, y);
	char c = expectedStone(x, y).color;

	if (s.color == c)
		return;

	fprintf(file,
	        "%d,%d: expected %c, found %c "
	        "(x: %d, y %d, brt: %f, sat: %f)\n",
	        x, y, c, s.color, s.x, s.y, s.brightness, s.saturation);
}

void Board::printExpected (FILE *file) {
	for (int i = 1; i <= grid.size; i++)
		for (int j = 1; j <= grid.size; j++)
			printStoneExpected(j, i, file);
}

std::vector< std::vector<Stone> > Board::readExpected(FILE *in) {
	std::vector< std::vector<Stone> > arr;
	for (int i = 0; i < grid.size; i++) {
		arr.push_back(std::vector<Stone>());

		for (int j = 0; j < grid.size; j++) {
			Stone s;
			int c;
			do {
				c = getc(in);
			} while (isspace(c));
			if (c == EOF)
				return arr;

			s.color = toupper(c);
			arr[i].push_back(s);
		}
	}

	return arr;
}

sf::Color colorAverage(std::vector<sf::Color> colors) {
	int count = colors.size();
	int r, g, b, a; // sf::Color would overflow
	r = g = b = a = 0;
	for (int i = 0; i < count; i++) {
		r += colors[i].r;
		g += colors[i].g;
		b += colors[i].b;
		a += colors[i].a;
	}

	return sf::Color(r/count, g/count, b/count, a/count);
}

float colorBrightness(sf::Color c) {
	return .30*c.r + .59*c.g + .11*c.b;
}

float colorSaturation(sf::Color c) {
	return ph::max3(c.r, c.g, c.b) - ph::min3(c.r, c.g, c.b);
}
