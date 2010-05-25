#include <SFML/Graphics.hpp>
#include "vector.h"
#include "board.h"

#include <vector>

Board board;

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

int main(int argc, char **argv) {
	char *filename = argv[1];

	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "seegoban");
	sf::Image board_image;
	if (! board_image.LoadFromFile(filename))
		return 1;

	board = Board(19, board_image);

	sf::Sprite board_sprite;
	board_sprite.SetImage(board_image);
	board_sprite.Resize(640, 480);

	while (window.IsOpened()) {
		handleEvents(window);

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
	case sf::Event::MouseButtonPressed: {
		ph::vec2f pt(e.MouseButton.X, e.MouseButton.Y);

		switch (e.MouseButton.Button) {
		case sf::Mouse::Left:
			if (!board.grid.defined)
				board.grid.corner(pt);
			else
				board.grid.moveNearest(pt);
			break;
		case sf::Mouse::Right:
			board.getStoneAtPoint(pt);
			ph::vec2f in = board.grid.getIntersection(15, 3);
			printf("%2.f/%.2f, %2.f/%.2f\n", pt.x, in.x, pt.y, in.y);

			break;
		}
		break;
	}
	case sf::Event::KeyPressed:
		switch (e.Key.Code) {
		case sf::Key::Escape:
			window.Close();
			break;
		case sf::Key::Return:
			board.printSgf(stdout);
			break;
		}
		break;
	}
}
