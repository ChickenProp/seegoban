#ifndef SGB_OPTIONS_H
#define SGB_OPTIONS_H

#include <cstdio>

class Options {
public:
	Options(int argc, char **argv);
	int index; // A way to access the value of getopt's 'optind' variable.

	bool autorun;
	char auto_output;

	bool expect;
	FILE *expect_file;

	int num_coords;
	float coords[4][2];
};

#endif
