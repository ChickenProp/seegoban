#include "stone.h"

Stone Stone::none(int x, int y, float b, float s) {
	return Stone('.', x, y, b, s);
}
Stone Stone::white(int x, int y, float b, float s) {
	return Stone('W', x, y, b, s);
}
Stone Stone::black(int x, int y, float b, float s) {
	return Stone('B', x, y, b, s);
}
