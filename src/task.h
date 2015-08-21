#ifndef TASK_H
#define TASK_H 
#include "metrics.h"
#include "bip.h"
#include "net.h"

typedef struct TASK {
	//common
	struct METRICS * (*alg)(struct TASK *);
	BIP* train;
	BIP* test;
	NET* trainr_cosine_similarity;
	int num_toprightused2cmptmetrics;

	//result;
	struct METRICS *mtc;
} TASK;

#endif
