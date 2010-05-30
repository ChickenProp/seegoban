#ifndef SGB_STONE_H
#define SGB_STONE_H

class Stone {
public:
	Stone() {}

	Stone(char c, int x, int y, float b, float s)
		: color(c), x(x), y(y), brightness(b), saturation(s) {}

	static Stone none(int x, int y, float b, float s);
	static Stone white(int x, int y, float b, float s);
	static Stone black(int x, int y, float b, float s);

	char color;
	int x, y;
	float brightness, saturation;
};

#endif
