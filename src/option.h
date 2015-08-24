#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

typedef struct OPTION {
	bool alg_mass;
	bool alg_hybrid;
	bool alg_ucf;
	bool alg_icf;

	char *filename_full;
	char *filename_train;
	char *filename_test;

	double rate_dividefulldataset;
	double rate_hybridparam;
	int num_looptimes;
	int num_toprightused2cmptmetrics;
	unsigned long num_randomseed;
	int num_ucf_knn;

	//
	char *logfilename;
} OPTION;

OPTION *setOPTION(int argc, char **argv);
void freeOPTION(OPTION *op);

#endif
