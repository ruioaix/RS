#ifndef OPTION_H
#define OPTION_H 

#include <stdbool.h>

typedef struct OPTION {
	bool alg_mass;
	bool alg_heats;
	bool alg_heats2;
	bool alg_hybrid;
	bool alg_ucf;
	bool alg_icf;
	bool alg_zm;
	bool alg_zmu;
	bool alg_zmuo;
	bool alg_kk;

	bool noIL;

	char *filename_full;
	char *filename_train;
	char *filename_test;

	double rate_dividefulldataset;
	double rate_huparam;
	double rate_hiparam;
	double rate_kuparam;
	double rate_kiparam;
	double rate_hybridparam;
	double rate_zmparam;
	double rate_zmuparam;
	double rate_zmuoparam;
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
