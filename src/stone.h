#ifndef SGB_STONE_H
#define SGB_STONE_H

#include "ph-utils/math-util.h"
#include <SFML/Graphics/Color.hpp>

class Stone {
public:
	Stone() {}

	Stone (char c, int x, int y, sf::Color rgb)
		: color(c), x(x), y(y), rgb(rgb) {}

	static Stone none(int x, int y, sf::Color rgb);
	static Stone white(int x, int y, sf::Color rgb);
	static Stone black(int x, int y, sf::Color rgb);

	char color;
	int x, y;
	sf::Color rgb;

	float brightness();
	static float brightness(sf::Color c);
	int saturation();
	static int saturation(sf::Color c);
};

#endif
