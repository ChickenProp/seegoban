#include <SFML/Graphics.hpp>
#include "vector.h"
#include "gridpoint.h"

#include <vector>

std::vector<GridPoint> corners;

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

int main(int argc, char **argv) {
	char *filename = argv[1];
	int boardsize = 19;

	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "seegoban");
	sf::Image board_image;
	if (! board_image.LoadFromFile(filename))
		return 1;

	sf::Sprite board_sprite;
	board_sprite.SetImage(board_image);
	board_sprite.Resize(640, 480);

	sf::Shape circle = sf::Shape::Circle(0, 0, 5, sf::Color(0, 0, 0, 0), 1, sf::Color(255, 0, 0));
	sf::Shape line;

	while (window.IsOpened()) {
		handleEvents(window);
		window.Draw(board_sprite);

		for (int i = 0; i < corners.size(); i++) {
			corners[i].render(window);
		}

		if (corners.size() >= 4) {
			ph::vec2f u = (corners[1] - corners[0])/(boardsize-1);
			ph::vec2f v = (corners[2] - corners[3])/(boardsize-1);
			for (int i = 0; i < boardsize; i++) {
				line = sf::Shape::Line(corners[0] + i*u,
				                       corners[3] + i*v,
				                       1, sf::Color(255,0,0));

				window.Draw(line);
			}

			u = (corners[3] - corners[0])/(boardsize-1.f);
			v = (corners[2] - corners[1])/(boardsize-1.f);
			for (int i = 0; i < boardsize; i++) {
				line = sf::Shape::Line(corners[0] + (float)i*u,
				                       corners[1] + (float)i*v,
				                       1, sf::Color(255,0,0));

				window.Draw(line);
			}
		}

		window.Display();
	}
}

void handleEvents(sf::RenderWindow &window) {
		sf::Event e;
		while (window.GetEvent(e)) {
			handleEvent(window, e);
		}
}

void handleEvent(sf::RenderWindow &window, sf::Event e) {
	switch (e.Type) {
	case sf::Event::Closed:
		window.Close();
	case sf::Event::MouseButtonPressed:
		if (corners.size() >= 4)
			break;
		corners.push_back(GridPoint(e.MouseButton.X, e.MouseButton.Y));
		break;
	case sf::Event::KeyPressed:
		switch (e.Key.Code) {
		case sf::Key::Escape:
			window.Close();
			break;
		case sf::Key::R:
			break;
		}
		break;
	}
}
