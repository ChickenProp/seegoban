#include <SFML/Graphics.hpp>
#include "vector.h"
#include "board.h"

#include <vector>

Board board;

void handleEvents(sf::RenderWindow &window);
void handleEvent(sf::RenderWindow &window, sf::Event e);

typedef enum { none, white, black } Stone;
Stone getStoneAtPoint(sf::Image image, ph::vec2f point);
std::vector<sf::Color> getSurroundingPixels(sf::Image image, int x, int y, int size);
sf::Color colorAverage(std::vector<sf::Color> colors);
float colorBrightness(sf::Color color);
float colorSaturation(sf::Color color);

bool rightclicked = false;
ph::vec2f rtclick;

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

		if (rightclicked) {
			rightclicked = false;
			getStoneAtPoint(board_image,
			                board_sprite.TransformToLocal(rtclick));
		}

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
			rightclicked = true;
			rtclick = pt;
			break;
		}
		break;
	}
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

Stone getStoneAtPoint(sf::Image image, ph::vec2f p) {
	int x = (int) p.x;
	int y = (int) p.y;

	sf::Color color = colorAverage(getSurroundingPixels(image, x, y, 6));
	printf("color near %d,%d is %d %d %d %d with %f/%f\n", x, y, color.r, color.g, color.b, color.a, colorBrightness(color), colorSaturation(color));
	return none;
}

std::vector<sf::Color> getSurroundingPixels(sf::Image image, int x, int y, int size) {
	std::vector<sf::Color> pixels;

	for (int i = x - size/2; i < x + size/2; i++) {
		for (int j = y - size/2; j < y + size/2; j++) {
			pixels.push_back(image.GetPixel(i, j));
		}
	}

	return pixels;
}

sf::Color colorAverage(std::vector<sf::Color>  colors) {
	int count = colors.size();
	int r, g, b, a; // sf::Color would overflow
	r = g = b = a = 0;
	for (int i = 0; i < count; i++) {
		r += colors[i].r;
		g += colors[i].g;
		b += colors[i].b;
		a += colors[i].a;
	}

	return sf::Color(r/count, g/count, b/count, a/count);
}

float colorBrightness(sf::Color c) {
	return .30*c.r + .59*c.g + .11*c.b;
}

int max3 (int a, int b, int c) {
	if (a > b)
		return a > c ? a : c;
	else
		return b > c ? b : c;
}

int min3 (int a, int b, int c) {
	if (a < b)
		return a < c ? a : c;
	else
		return b < c ? b : c;
}
			

float colorSaturation(sf::Color c) {
	return max3(c.r, c.g, c.b) - min3(c.r, c.g, c.b);
}
