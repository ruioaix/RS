#include "log.h"
#include "tasklist.h"

int main(int argc, char **argv) {
	setloglevel(LOG_DBG);

	struct OPTION *op = setOPTION(argc, argv);
	struct TASKLIST *tl = walkingTL(op);
	freeOPTION(op);

	int i;
	for (i = 0; i < tl->num; ++i) {
		struct METRICS *result = tl->core[i]->mtc;
		LOG(LOG_OP, "METRICS: R: %f, RL: %f, PL: %f, HL: %f, IL: %f, NL: %f.", result->R, result->RL, result->PL, result->HL, result->IL, result->NL);
	}
	freeTL(tl);
	releaselog();
	return 0;
}
