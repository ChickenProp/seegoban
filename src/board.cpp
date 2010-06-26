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
//	printf("color near %d,%d is %d %d %d %d with %f/%f\n", x, y, color.r, color.g, color.b, color.a, colorBrightness(color), colorSaturation(color));

	Stone c;

	if (sat > 50)
		c = Stone::none(x, y, brt, sat);
	else if (brt < 150)
		c = Stone::black(x, y, brt, sat);
	else
		c = Stone::white(x, y, brt, sat);
		
	return c;
}

std::vector<sf::Color> Board::getSurroundingPixels(int x, int y, int size) {
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

void Board::printText (FILE *file) {
	for (int i = 1; i <= grid.size; i++) {
		for (int j = 1; j <= grid.size; j++) {
			ph::vec2f pt = grid.getIntersection(j, i);
			fprintf(file, "%c ", getStoneAtPoint(pt).color);
		}
		fprintf(file, "\n");
	}
}

void Board::printSgf (FILE *file) {
	// I don't really understand SGF, this is taken from perl's Image2SGF.
	fprintf(file, "(;GM[1]FF[4]SZ[%d]\n", grid.size);
	// PL[B] means it's black's turn to play, which is an okay default.
	fprintf(file, "GN[seegoban conversion]\n\nAP[seegoban]\nPL[B]\n\n");

	for (int i = 1; i <= grid.size; i++) {
		for (int j = 1; j <= grid.size; j++) {
			ph::vec2f pt = grid.getIntersection(j, i);
			Stone s = getStoneAtPoint(pt);
			char c = s.color;
			if (c == '.') continue;

			char x = '`' + j; // '`' is the character before 'a'
			char y = '`' + i;
			fprintf(file, "A%c[%c%c]C[%d,%d]\n", c, x, y, s.x, s.y);
		}
	}
	fprintf(file, ")\n");
	fflush(file);
}

void Board::printDebug (FILE *file) {
	for (int i = 1; i <= grid.size; i++) {
		for (int j = 1; j <= grid.size; j++) {
			ph::vec2f pt = grid.getIntersection(j, i);
			Stone s = getStoneAtPoint(pt);

			if (hasExpect)
				fprintf(file,
				        "%d\t%d\t%c\t%d\t%d\t%f\t%f\t%c\n",
				        j, i, s.color, s.x, s.y,
				        s.brightness, s.saturation,
				        expectedStones[i-1][j-1].color);
			else
				fprintf(file,
				        "%d\t%d\t%c\t%d\t%d\t%f\t%f\n",
				        j, i, s.color, s.x, s.y,
				        s.brightness, s.saturation);
		}
	}
}

void Board::printExpected (FILE *out) {
	for (int i = 1; i <= grid.size; i++) {
		for (int j = 1; j <= grid.size; j++) {
			ph::vec2f pt = grid.getIntersection(j, i);
			Stone s = getStoneAtPoint(pt);
			char c = expectedStones[i-1][j-1].color;

			if (s.color == c)
				continue;

			fprintf(out, "%d,%d: expected %c, found %c (x: %d, y %d, brt: %f, sat: %f)\n",
			        j, i, c, s.color, s.x, s.y,
			        s.brightness, s.saturation);
		}
	}
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
