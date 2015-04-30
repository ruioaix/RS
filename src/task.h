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

	//used to improve efficient
	int *rank;
	
	double *lvlts = smalloc((trainl->maxId + 1)*sizeof(double));
	double *rvlts = smalloc((trainr->maxId + 1)*sizeof(double));
	int *lidts= smalloc((trainl->maxId + 1)*sizeof(int));
	int *ridts = smalloc((trainr->maxId + 1)*sizeof(int));

	//result;
	struct METRICS *mtc;
};

#endif
