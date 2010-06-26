#ifndef SGB_BOARD_H
#define SGB_BOARD_H

#include <SFML/Graphics.hpp>
#include "grid.h"
#include "stone.h"
#include <cstdio>

class Board {
public:
	Board();
	Board(int size, const sf::Image &img, FILE *expect = NULL);

	Grid grid;
	sf::Image image;
	sf::Sprite sprite;
	sf::View view;

	void render(sf::RenderTarget &target);

	bool hasExpect;
	std::vector< std::vector<Stone> > expectedStones;

	Stone getStoneAtPoint(ph::vec2f point);
	std::vector<sf::Color> getSurroundingPixels(int x, int y, int size);

	void openInCgoban();
	void printSgf (FILE *file);
	void printText (FILE *file);
	void printDebug (FILE *file);
	void printExpected (FILE *out);

	std::vector< std::vector<Stone> > readExpected(FILE *in);
};

#endif
