#include "board.h"

Board::Board() {}
Board::Board(int size, const sf::Image &img)
	: grid(size), image(img), sprite()
{
	sprite.SetImage(img); // 'image' here doesn't seem to work?
	sprite.Resize(640, 480);
}

void Board::render (sf::RenderTarget &tgt) {
	tgt.Draw(sprite);
	grid.render(tgt);
}
