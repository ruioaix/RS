#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

struct OPTION {
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

	//
	char *logfilename;
};

struct OPTION *setOPTION(int argc, char **argv);
int algnumOPTION(struct OPTION *op);
void freeOPTION(struct OPTION *op);

#endif
