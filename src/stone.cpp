#include "stone.h"

Stone Stone::none(int x, int y, sf::Color rgb) {
	return Stone('.', x, y, rgb);
}
Stone Stone::white(int x, int y, sf::Color rgb) {
	return Stone('W', x, y, rgb);
}
Stone Stone::black(int x, int y, sf::Color rgb) {
	return Stone('B', x, y, rgb);
}

float Stone::brightness() {
	return Stone::brightness(rgb);
}
float Stone::brightness(sf::Color c) {
	return .30*c.r + .59*c.g + .11*c.b;
}

int Stone::saturation() {
	return Stone::saturation(rgb);
}
int Stone::saturation(sf::Color c) {
	return ph::max3(c.r, c.g, c.b) - ph::min3(c.r, c.g, c.b);
}
