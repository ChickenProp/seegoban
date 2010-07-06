#include <SFML/Graphics.hpp>
#include "ph-utils/vector.h"
#include "board.h"
#include "unistd.h"
#include <cstring>
#include <vector>

Board board;

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

int main(int argc, char **argv) {
	bool opt_auto = false;
	bool opt_expect = false;
	FILE *expect_file = NULL;
	char opt_output = 's';
	const int max_coords = 5;
	int num_coords = 0;
	float coords[max_coords][2];
	int c;
	while ((c = getopt(argc, argv, "tsdac:e:")) != -1) {
		switch (c) {
		case 'a':
			opt_auto = true;
			break;
		case 't': 
		case 's': 
		case 'd': 
			opt_output = c;
			break;
		case 'c': {
			coords[num_coords][0] = atof(optarg);
			char *second = strchr(optarg, ',');
			coords[num_coords][1] = atoi(second+1);
			num_coords++;
			break;
		}
		case 'e':
			opt_output = 'e';
			opt_expect = true;
			if (strcmp(optarg, "-") == 0)
				expect_file = stdin;
			else
				expect_file = fopen(optarg, "r");
			break;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Must supply a board image.\n");
		exit(1);
	}

	char *filename = argv[optind];

	sf::Image board_image;
	if (! board_image.LoadFromFile(filename))
		return 1;

	if (!opt_expect) 
		board = Board(19, board_image);
	else
		board = Board(19, board_image, expect_file);

	for (int i = 0; i < num_coords; i++)
		board.grid.corner(coords[i][0], coords[i][1]);

	if (opt_auto) {
		if (!board.grid.defined) {
			fprintf(stderr, "All four corners must be defined.\n");
			exit(1);
		}

		switch (opt_output) {
		case 't':
			board.printText(stdout);
			break;
		case 's':
			board.printSgf(stdout);
			break;
		case 'd':
			board.printDebug(stdout);
			break;
		case 'e':
			board.printExpected(stdout);
			break;
		}

		exit(0);
	}

	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "seegoban");
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

	const sf::Input &in = window.GetInput();
	if (in.IsMouseButtonDown(sf::Mouse::Left) && board.grid.defined) {
		ph::vec2f pt(window.ConvertCoords(in.GetMouseX(),
		                                  in.GetMouseY()));
		board.grid.moveNearest(pt);
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
			board.printText(stdout);
			break;
		case sf::Key::S:
			board.printSgf(stdout);
			break;
		case sf::Key::D:
			board.printDebug(stdout);
			break;
		case sf::Key::C:
			board.openInCgoban();
			break;
		case sf::Key::E:
			board.printExpected(stdout);
			break;
		case sf::Key::G:
			board.printXGraph(stdout);
			break;
		case sf::Key::V:
			board.viewGraph();
			break;
		}
		break;
	}
}
