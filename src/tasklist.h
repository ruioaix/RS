#ifndef TASKLIST_H
#define TASKLIST_H 

#include "task.h"
#include "option.h"

typedef struct TASK *(*ALGS)(struct OPTION *);

typedef struct TASKLIST {
	int listNum;
	int currNum;
	TASK **list;

	//share
	BIP *train;
	BIP *test;
	NET *trainr_cosine_similarity;
	double *dt;

	//private
	ALGS *algs;
	int algsNum;

} TASKLIST;

void freeTL(TASKLIST *tl);

TASKLIST *walkingTL(OPTION *op);

#endif
