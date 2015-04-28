#include "task.h"
#include <stdlib.h>

void freeTASK(struct TASK *task) {
	freeBIPS(task->train);
	freeBIPS(task->test);
	freeNETS(task->trainr_cosin_similarity);
	free(task);
}
