#include "gridpoint.h"

void GridPoint::render(sf::RenderTarget &tgt) {
	sf::Shape circle = sf::Shape::Circle(x, y, 5, sf::Color(0, 0, 0, 0),
	                                     1, sf::Color(255, 0, 0));
	tgt.Draw(circle);
}
