#include <SFML/Graphics.hpp>
#include "vector.h"
#include "board.h"
#include "unistd.h"
#include <cstring>
#include <vector>

Board board;

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

int main(int argc, char **argv) {
	bool opt_auto = false;
	char opt_output = 's';
	int num_corners = 0;
	int corners[2][5];
	int c;
	while ((c = getopt(argc, argv, "tsdac:")) != -1) {
		switch (c) {
		case 'a':
			opt_auto = true;
			break;
		case 't': 
		case 's': 
		case 'd': 
			opt_output = c;
			break;
		case 'c':
			corners[num_corners][0] = atoi(optarg);
			char *second = strchr(optarg, ',');
			corners[num_corners][1] = atoi(second+1);
			num_corners++;
			break;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Must supply a board image.\n");
		exit(1);
	}

	for (int i = 0; i < num_corners; i++)
		printf("corner %d,%d\n", corners[i][0], corners[i][1]);

	char *filename = argv[optind];

	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "seegoban");
	sf::Image board_image;
	if (! board_image.LoadFromFile(filename))
		return 1;

	board = Board(19, board_image);

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
			Stone s = board.getStoneAtPoint(pt);
			printf("(%d, %d) %c: %f, %f\n", s.x, s.y, s.color,
			       s.brightness, s.saturation);
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
