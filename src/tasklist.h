#ifndef TASKLIST_H
#define TASKLIST_H 

#include "task.h"
#include "option.h"

typedef struct TASK OTL;

struct TASKLIST {
	int num;
	OTL **core;

	//private
	//share
	BIPS *train;
	BIPS *test;
	NETS *trainr_cosine_similarity;

};

void freeOTL(OTL *otl);
void freeTL(struct TASKLIST *tl);
struct TASKLIST *walkingTL(struct OPTION *op);

#endif
