#include "log.h"
#include "tasklist.h"

int main(int argc, char **argv) {
	setloglevel(LOG_INFO);

	struct OPTION *op = setOPTION(argc, argv);
	struct TASKLIST *tl = walkingTL(op);
	freeOPTION(op);

	int i;
	for (i = 0; i < tl->listNum; ++i) {
		struct METRICS *result = tl->core[i]->mtc;
		LOG(LOG_OP, "METRICS: R: %f, RL: %f, PL: %f, HL: %f, IL: %f, NL: %f, SL: %f.", result->R, result->RL, result->PL, result->HL, result->IL, result->NL, result->SL);
	}
	freeTL(tl);
	releaselog();
	return 0;
}
