#include <SFML/Graphics.hpp>
#include "vector.h"
#include "grid.h"

#include <vector>

Grid board(19);

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

int main(int argc, char **argv) {
	char *filename = argv[1];

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

		board.render(window);

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
		if (!board.defined)
			board.corner(e.MouseButton.X, e.MouseButton.Y);
		else
			board.moveNearest(e.MouseButton.X, e.MouseButton.Y);
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
