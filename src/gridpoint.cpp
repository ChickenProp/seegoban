#include "gridpoint.h"

void GridPoint::render(sf::RenderTarget &tgt) {
// We don't want the circle to scale with the image, so we have to break out of
// the current View and compute the position manually.

	sf::View view = tgt.GetView();
	sf::FloatRect rect = view.GetRect();

	float xtrans = rect.Left + (x * tgt.GetWidth() / rect.GetWidth());
	float ytrans = rect.Top + (y * tgt.GetHeight() / rect.GetHeight());

	tgt.SetView( tgt.GetDefaultView() );
	
	sf::Shape circle = sf::Shape::Circle(xtrans, ytrans, 5,
	                                     sf::Color(0, 0, 0, 0),
	                                     1, sf::Color(255, 0, 0));
	tgt.Draw(circle);

	tgt.SetView(view);
}
