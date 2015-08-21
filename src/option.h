#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

typedef struct OPTION {
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
} OPTION;

OPTION *setOPTION(int argc, char **argv);
int algnumOPTION(OPTION *op);
void freeOPTION(OPTION *op);

#endif
