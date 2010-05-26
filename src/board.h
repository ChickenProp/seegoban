#ifndef SGB_BOARD_H
#define SGB_BOARD_H

#include <SFML/Graphics.hpp>
#include "grid.h"
#include <cstdio>

class Stone {
public:
	Stone() {}

	Stone(char c, int x, int y, float b, float s)
		: color(c), x(x), y(y), brightness(b), saturation(s) {}

	static Stone none(int x, int y, float b, float s);
	static Stone white(int x, int y, float b, float s);
	static Stone black(int x, int y, float b, float s);

	char color;
	int x, y;
	float brightness, saturation;
};

class Board {
public:
	Board();
	Board(int size, const sf::Image &img);

	Grid grid;
	sf::Image image;

	sf::Sprite sprite;

	void render(sf::RenderTarget &target);

	Stone getStoneAtPoint(ph::vec2f point);
	std::vector<sf::Color> getSurroundingPixels(int x, int y, int size);

	void printSgf (FILE *file);
	void printText (FILE *file);
};

#endif
