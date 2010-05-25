#ifndef SGB_BOARD_H
#define SGB_BOARD_H

#include <SFML/Graphics.hpp>
#include "grid.h"
#include <cstdio>

class Stone {
public:
	Stone() : color('.') {}
	Stone(char c) : color(c) {}

	static const Stone none;
	static const Stone white;
	static const Stone black;
	
	char color;
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
