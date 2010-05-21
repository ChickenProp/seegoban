#include <SFML/Graphics.hpp>

sf::Vector2f corners[4];
int current_corner = 0;

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

	while (window.IsOpened()) {
		handleEvents(window);
		window.Draw(board_sprite);
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
		if (current_corner >= 4)
			break;
		corners[current_corner] = sf::Vector2f(e.MouseButton.X,
		                                       e.MouseButton.Y);
		current_corner++;
		break;
	}
}
