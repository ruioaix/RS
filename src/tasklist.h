#ifndef TASKLIST_H
#define TASKLIST_H 

#include "task.h"
#include "option.h"

struct TASKLIST {
	int num;
	struct TASK **core;
};

void freeTL(struct TASKLIST *tl);
struct TASKLIST *createTL(struct OPTION *op);

#endif
