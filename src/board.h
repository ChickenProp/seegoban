#ifndef SGB_BOARD_H
#define SGB_BOARD_H

#include <SFML/Graphics.hpp>
#include "grid.h"

class Stone {
public:
	enum Color { none, white, black };
};

class Board {
public:
	Board();
	Board(int size, const sf::Image &img);

	Grid grid;
	sf::Image image;

	sf::Sprite sprite;

	void render(sf::RenderTarget &target);

	Stone::Color getStoneAtPoint(ph::vec2f point);
	std::vector<sf::Color> getSurroundingPixels(int x, int y, int size);
};

#endif
