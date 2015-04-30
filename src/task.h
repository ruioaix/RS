#ifndef TASK_H
#define TASK_H 
#include "metrics.h"
#include "bip.h"
#include "net.h"
#include "option.h"

struct TASK {
	struct METRICS * (*alg)(struct TASK *);
	BIPS* train;
	BIPS* test;
	NETS* trainr_cosine_similarity;
	int num_toprightused2cmptmetrics;

	//result;
	struct METRICS *mtc;
};

#endif
