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

	float blackBrtMax;
	float whiteBrtMin;
	float colSatMax;
	void setThresholds(float *thresholds);

	void render(sf::RenderTarget &target);

	bool hasExpect;
	Stone expectedStone(int x, int y);

	int sampleSize;
	Stone getStoneAtPoint(ph::vec2f point);
	Stone getStoneAtIntersection(int x, int y);
	std::vector<sf::Color> getSurroundingPixels(int x, int y, int size);

	void openInCgoban();
	void viewGraph();
	void printSgf (FILE *file);
	void printText (FILE *file);
	void printDebug (FILE *file);
	void printXGraph (FILE *file);
	void printExpected (FILE *file);

	std::vector< std::vector<Stone> > readExpected(FILE *in);

	void printStoneSgf (int x, int y, FILE *file);
	void printStoneDebug (int x, int y, FILE *file);
	void printStoneExpected (int x, int y, FILE *file);

	std::vector< std::vector<Stone> > expectedStones;
};

#endif
