#ifndef SGB_BOARD_H
#define SGB_BOARD_H

#include <SFML/Graphics.hpp>
#include "grid.h"

class Stone {
public:
	enum Color { S_NONE, S_WHITE, S_BLACK };

	Stone() : color(S_NONE) {}
	Stone(Color c) : color(c) {}

	static const Stone none;
	static const Stone white;
	static const Stone black;
	
	Color color;

	operator const char*() {
		return (color == S_NONE ? "none" :
		        color == S_WHITE ? "white" : "black");
	}
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
};

#endif
