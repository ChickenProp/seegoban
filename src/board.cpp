#include "board.h"

sf::Color colorAverage(std::vector<sf::Color> colors);
float colorBrightness(sf::Color color);
float colorSaturation(sf::Color color);
int max3 (int a, int b, int c);
int min3 (int a, int b, int c);

const Stone Stone::none(S_NONE);
const Stone Stone::white(S_WHITE);
const Stone Stone::black(S_BLACK);

Board::Board() {}
Board::Board(int size, const sf::Image &img)
	: grid(size), image(img), sprite()
{
	sprite.SetImage(img); // 'image' here doesn't seem to work?
	sprite.Resize(640, 480);
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
		c = Stone::none;
	else
		c = (brt < 150 ? Stone::black : Stone::white);
		
	printf("%s (%f, %f)\n", (const char *)c, brt, sat);
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
	return max3(c.r, c.g, c.b) - min3(c.r, c.g, c.b);
}

int max3 (int a, int b, int c) {
	if (a > b)
		return a > c ? a : c;
	else
		return b > c ? b : c;
}

int min3 (int a, int b, int c) {
	if (a < b)
		return a < c ? a : c;
	else
		return b < c ? b : c;
}
