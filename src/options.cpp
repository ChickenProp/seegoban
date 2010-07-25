#include "options.h"
#include "unistd.h"
#include <cstdlib>
#include <cstring>

Options::Options (int argc, char **argv)
	: autorun(false),
	  expect(false),
	  num_coords(0),
	  given_thresholds(false),
	  sample_size(0)
{
	int c;
	while ((c = getopt(argc, argv, "a:c:e:s:t:")) != -1) {
		switch (c) {
		case 'a':
			autorun = true;
			auto_output = optarg[0];
			break;
		case 'c': {
			coords[num_coords][0] = atof(optarg);
			char *second = strchr(optarg, ',');
			coords[num_coords][1] = atof(second+1);
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
		case 's':
			sample_size = atoi(optarg);
			break;
		case 't': {
			given_thresholds = true;
			thresholds[0] = atof(optarg);
			char *second = strchr(optarg, ',') + 1;
			thresholds[1] = atof(second);
			char *third = strchr(second, ',') + 1;
			thresholds[2] = atof(third);
		}
		}
	}

	index = optind;
}

	
