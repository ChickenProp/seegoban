#ifndef SGB_BOARD_H
#define SGB_BOARD_H

#include <SFML/Graphics.hpp>
#include "grid.h"
#include "stone.h"
#include <cstdio>

class Board {
public:
	Board();
	Board(int size, const sf::Image &img);

	Grid grid;
	sf::Image image;
	sf::Sprite sprite;
	sf::View view;

	void render(sf::RenderTarget &target);

	Stone getStoneAtPoint(ph::vec2f point);
	std::vector<sf::Color> getSurroundingPixels(int x, int y, int size);

	void printSgf (FILE *file);
	void printText (FILE *file);
};

#endif
