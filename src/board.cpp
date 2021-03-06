#include "board.h"
#include "ph-utils/math-util.h"

sf::Color colorAverage(std::vector<sf::Color> colors);

Board::Board() {}
Board::Board(int size, const sf::Image &img, FILE *expect)
	: grid(size), image(img), sprite(), view(),
	  hasExpect(false), expectedStones(),
	  blackBrtMax(159), whiteBrtMin(160), colSatMax(65),
	  sampleSize(7)
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

void Board::setThresholds(float *thresholds) {
	blackBrtMax = thresholds[0];
	whiteBrtMin = thresholds[1];
	colSatMax = thresholds[2];
}

void Board::render (sf::RenderTarget &tgt) {
	tgt.Draw(sprite);
	grid.render(tgt);
}

Stone Board::getStoneAtPoint(ph::vec2f p) {
	ph::vec2f l = sprite.TransformToLocal(p);
	int x = (int) l.x;
	int y = (int) l.y;

	sf::Color avg = colorAverage(getSurroundingPixels(x, y, sampleSize));
	float brt = Stone::brightness(avg);
	int sat = Stone::saturation(avg);

	Stone c;

	if (sat > colSatMax)
		c = Stone::none(x, y, avg);
	else if (brt <= blackBrtMax)
		c = Stone::black(x, y, avg);
	else if (brt >= whiteBrtMin)
		c = Stone::white(x, y, avg);
	else
		c = Stone::none(x, y, avg);
		
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

// Get the (size^2) pixels in a square centred on (x,y). If size is even, the
// centre of the square is at the bottom right of the (x,y) pixel.
std::vector<sf::Color> Board::getSurroundingPixels(int x, int y, int size) {
	std::vector<sf::Color> pixels;

	for (int i = x - (size-1)/2; i < x + size/2; i++) {
		for (int j = y - (size-1)/2; j < y + size/2; j++) {
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

// We can't avoid forking here: pclose() waits until the process finishes, so if
// I tried to use popen() without forking, seegoban would hang. (This isn't a
// problem with cgoban because that forks itself.) I tried to fork and then use
// popen(), but it gave continous weird errors if xgraph wasn't closed properly.
// This seems to work better, although it's more complicated.
void Board::viewGraph () {
	int stream[2];
	if (pipe(stream) < 0) {
		fprintf(stderr, "There was an error opening a stream.\n");
		return;
	}

	pid_t pid = fork();

	if (pid == -1) {
		fprintf(stderr, "There was an error forking in viewGraph().\n");
		return;
	}
	else if (pid == 0) {
		close(0);
		close(stream[1]);
		dup2(stream[0], 0);

		execlp("xgraph", "xgraph", "-nl", "-P", (char *) 0);

		fprintf(stderr, "Could not open xgraph for some reason.\n");
		exit(1);
	}
	else {
		close(stream[0]);

		FILE *out = fdopen(stream[1], "w");
		printXGraph(out);
		fclose(out);
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

	fprintf(file,
	        "%2d %2d     "
	        "%4d %4d     "
	        "%3d  %3d  %3d     "
	        "%7.3f  %3d     "
	        "%c %c\n",
	        x, y, s.x, s.y, s.rgb.r, s.rgb.g, s.rgb.b,
	        s.brightness(), s.saturation(),
	        s.color, (hasExpect ? expectedStone(x, y).color : ' '));
}

void Board::printDebug (FILE *file) {
	for (int i = 1; i <= grid.size; i++)
		for (int j = 1; j <= grid.size; j++)
			printStoneDebug(j, i, file);
}

void printXGraphSet (std::vector<Stone> stones, const char *name, FILE *file) {
	fprintf(file, "\"%s\"\n", name);

	for (int i = 0; i < stones.size(); i++)
		fprintf(file, "%f %d\n",
		        stones[i].brightness(), stones[i].saturation());

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
	        "(x: %d, y %d, brt: %f, sat: %d)\n",
	        x, y, c, s.color, s.x, s.y, s.brightness(), s.saturation());
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
