#include "log.h"
#include "tasklist.h"

int loglevel = LOG_INFO;
FILE *logstream;

int main(int argc, char **argv) {
	//dbgstream = fopen("/tmp/rdsm.log", "w");
	logstream = stdout;
	struct OPTION *op = setOPTION(argc, argv);
	struct TASKLIST *tl = createTL(op);

	struct TASK *task = tl->core;
	struct METRICS *result = task->alg(task);
	LOG(LOG_INFO, "METRICS: R: %f, RL: %f, PL: %f, HL: %f, IL: %f, NL: %f.", result->R, result->RL, result->PL, result->HL, result->IL, result->NL);
	return 0;
}
