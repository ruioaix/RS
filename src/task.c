#include "task.h"
#include <stdlib.h>

void freeTASK(struct TASK *task) {
	freeBIPS(task->train);
	freeBIPS(task->test);
	freeNETS(task->trainr_cosine_similarity);
	freeMTC(task->mtc);
	free(task);
}
