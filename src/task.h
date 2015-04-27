#ifndef TASK_H
#define TASK_H 
#include "metrics.h"

struct TASK {
	struct METRICS * (*alg)(struct TASK *);

};

#endif
