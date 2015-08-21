#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

struct OPTION {
	bool alg_mass;

	char *filename_full;
	char *filename_train;
	char *filename_test;

	double rate_dividefulldataset;
	int num_looptimes;
	int num_toprightused2cmptmetrics;
	unsigned long num_randomseed;

	//
	char *logfilename;
};

struct OPTION *setOPTION(int argc, char **argv);
int algnumOPTION(struct OPTION *op);
void freeOPTION(struct OPTION *op);

#endif
