#include "tasklist.h"
#include "bip.h"
#include "net.h"
#include "linefile.h"
#include "similarity.h"
#include "alg_mass.h"


struct TASKLIST *createTL(struct OPTION *op) {
	struct TASKLIST *tl = smalloc(sizeof(struct TASKLIST));
	int num = algnumOPTION(op); 

	if (!op->filename_full) {
		tl->core = smalloc(num * sizeof(struct TASK *));

		struct LineFile *trainf = createLF(op->filename_train, 1, 1, -1);
		struct LineFile *testf = createLF(op->filename_test, 1, 1, -1);
		BIPS *train = createBIPS(trainf);
		BIPS *test = createBIPS(testf);
		struct LineFile *simf = cosineSM(train->core[1], train->core[0]);
		NETS *trainr_cosin_similarity = createNETS(simf);
		freeLF(trainf);
		freeLF(testf);

		if (op->alg_mass) {
			tl->core->alg = alg_mass;
			tl->core->train = train;
			tl->core->test = test;
			tl->core->trainr_cosin_similarity = trainr_cosin_similarity;
			tl->core->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
		}

	}
	else {
		tl->core = smalloc(num * sizeof(struct TASK *));
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

		if (op->alg_mass) {
			tl->core->alg = alg_mass;
			tl->core->train = train;
			tl->core->test = test;
			tl->core->trainr_cosin_similarity = trainr_cosin_similarity;
			tl->core->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
		}

	}
	return tl;
}
