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
	const int max_coords = 5;
	int num_coords = 0;
	float coords[2][max_coords];
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
			coords[num_coords][0] = atof(optarg);
			char *second = strchr(optarg, ',');
			coords[num_coords][1] = atoi(second+1);
			num_coords++;
			break;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Must supply a board image.\n");
		exit(1);
	}

	for (int i = 0; i < num_coords; i++)
		printf("corner %f,%f\n", coords[i][0], coords[i][1]);

	char *filename = argv[optind];

	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "seegoban");
	sf::Image board_image;
	if (! board_image.LoadFromFile(filename))
		return 1;

	board = Board(19, board_image);

	for (int i = 0; i < num_coords; i++)
		board.grid.corner(coords[i][0], coords[i][1]);

	window.SetView(board.view);

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
		ph::vec2f pt(window.ConvertCoords(e.MouseButton.X,
		                                  e.MouseButton.Y));

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
		case sf::Key::C:
			board.openInCgoban();
			break;
		}
		break;
	}
}
