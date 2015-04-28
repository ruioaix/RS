#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

struct OPTIONS {
	bool alg_mass;
	bool alg_heats;
	bool alg_hybrid;
	bool alg_HNBI;

	char *filename_full;
	char *filename_train;
	char *filename_test;
	char *filename_leftobjectattr;

	double rate_dividefulldataset;
	int num_looptimes;
	int num_toprightused2cmptmetrics;
	unsigned long seed_random;
};

struct OPTIONS *setOPTOINS(int argc, char **argv);

#endif
