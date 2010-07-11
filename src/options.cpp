#include "options.h"
#include "unistd.h"
#include <cstdlib>
#include <cstring>

Options::Options (int argc, char **argv)
  : autorun(false), expect(false), num_coords(0)
{
	int c;
	while ((c = getopt(argc, argv, "a:c:e:")) != -1) {
		switch (c) {
		case 'a':
			autorun = true;
			auto_output = optarg[0];
			break;
		case 'c': {
			coords[num_coords][0] = atof(optarg);
			char *second = strchr(optarg, ',');
			coords[num_coords][1] = atoi(second+1);
			num_coords++;
			break;
		}
		case 'e':
			expect = true;
			if (strcmp(optarg, "-") == 0)
				expect_file = stdin;
			else
				expect_file = fopen(optarg, "r");
			break;
		}
	}

	index = optind;
}

	
