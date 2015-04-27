#ifndef TASK_H
#define TASK_H 
#include "metrics.h"
#include "bip.h"
#include "net.h"

struct TASK {
	struct METRICS * (*alg)(struct TASK *);
	BIPS* train;
	BIPS* test;
	NETS* trainr_cosin_similarity;
	int num_toprightused2cmptmetrics;

	//used to improve efficient
	int *rank;
};

#endif
