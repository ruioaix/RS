#include "log.h"
#include "tasklist.h"

int main(int argc, char **argv) {
	setloglevel(LOG_DBG);

	struct OPTION *op = setOPTION(argc, argv);
	struct TASKLIST *tl = walkingTL(op);
	freeOPTION(op);

	struct METRICS *result = tl->core[0]->mtc;
	LOG(LOG_OP, "METRICS: R: %f, RL: %f, PL: %f, HL: %f, IL: %f, NL: %f.", result->R, result->RL, result->PL, result->HL, result->IL, result->NL);
	freeTL(tl);
	releaselog();
	return 0;
}
