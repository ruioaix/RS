#include "tasklist.h"
#include "bip.h"
#include "log.h"
#include "net.h"
#include "linefile.h"
#include "similarity.h"
#include "alg_mass.h"
#include <stdlib.h>


void freeTL(struct TASKLIST *tl) {
	if (!tl) return;
	int i;
	for (i = 0; i < tl->num; ++i) {
		freeTASK(tl->core[i]);
	}
	free(tl->core);
	free(tl);
}

struct TASKLIST *createTL(struct OPTION *op) {
	struct TASKLIST *tl = smalloc(sizeof(struct TASKLIST));
	int num = 1;

	if (!op->filename_full) {
		tl->core = smalloc(num * sizeof(struct TASK *));
		tl->num = num;

		struct TASK *task = smalloc(sizeof(struct TASK));

		struct LineFile *trainf = createLF(op->filename_train, 1, 1, -1);
		struct LineFile *testf = createLF(op->filename_test, 1, 1, -1);
		BIPS *train = createBIPS(trainf);
		BIPS *test = createBIPS(testf);
		struct LineFile *simf = cosineSM(train->core[1], train->core[0]);
		NETS *trainr_cosin_similarity = createNETS(simf);
		freeLF(simf);
		freeLF(trainf);
		freeLF(testf);

		if (op->alg_mass) {
			task->alg = alg_mass;
			task->train = train;
			task->test = test;
			task->trainr_cosin_similarity = trainr_cosin_similarity;
			task->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
			tl->core[0] = task;
		}

	}
	else {
		tl->core = smalloc(num * sizeof(struct TASK *));
		tl->num = num;

		struct TASK *task = smalloc(sizeof(struct TASK));

		struct LineFile *lf = createLF(op->filename_full, 1, 1, -1);
		BIP *left = createBIP(lf, LEFT);
		BIP *right = createBIP(lf, RIGHT);
		freeLF(lf);
		struct LineFile *testf, *trainf;
		divideBIP(left, right, op->rate_dividefulldataset, &testf, &trainf);
		freeBIP(left);
		freeBIP(right);
		BIPS *train = createBIPS(trainf);
		freeLF(trainf);
		BIPS *test = createBIPS(testf);
		freeLF(testf);
		struct LineFile *simf = cosineSM(train->core[1], train->core[0]);
		NETS *trainr_cosin_similarity = createNETS(simf);
		freeLF(simf);

		if (op->alg_mass) {
			task->alg = alg_mass;
			task->train = train;
			task->test = test;
			task->trainr_cosin_similarity = trainr_cosin_similarity;
			task->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
			tl->core[0] = task;
		}

	}
	LOG(LOG_INFO, "tasklist created.");
	return tl;
}
