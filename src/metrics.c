#include "metrics.h"
#include "utils.h"
#include <stdlib.h>

struct METRICS *createMTC(void) {
	struct METRICS *lp = smalloc(sizeof(struct METRICS));
	lp->R = 0;
	lp->RL = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->COV = 0;
	return lp;
}

void freeMTC(struct METRICS *lp) {
	free(lp);
}
