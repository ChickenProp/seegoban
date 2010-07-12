#include <SFML/Graphics.hpp>
#include "ph-utils/vector.h"
#include "board.h"
#include "options.h"
#include <cstring>
#include <vector>

Board board;

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

int main(int argc, char **argv) {
	Options opts(argc, argv);

	if (opts.index >= argc) {
		fprintf(stderr, "Must supply a board image.\n");
		exit(1);
	}

	char *filename = argv[opts.index];

	sf::Image board_image;
	if (! board_image.LoadFromFile(filename))
		return 1;

	if (!opts.expect) 
		board = Board(19, board_image);
	else
		board = Board(19, board_image, opts.expect_file);

	if (opts.given_thresholds)
		board.setThresholds(opts.thresholds);

	for (int i = 0; i < opts.num_coords; i++)
		board.grid.corner(opts.coords[i][0], opts.coords[i][1]);

	if (opts.autorun) {
		if (!board.grid.defined) {
			fprintf(stderr, "All four corners must be defined.\n");
			exit(1);
		}

		switch (opts.auto_output) {
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
		case 'c':
			board.openInCgoban();
			break;
		case 'g':
			board.printXGraph(stdout);
			break;
		case 'v':
			board.viewGraph();
			break;
		default:
			fprintf(stderr, "Unknown output format %c.\n",
			        opts.auto_output);
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
		case sf::Key::P:
			board.grid.printCorners(stdout);
			break;
		}
		break;
	}
}
