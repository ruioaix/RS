#include "tasklist.h"
#include "random.h"
#include "bip.h"
#include "log.h"
#include "net.h"
#include "linefile.h"
#include "similar.h"
#include "mass.h"
#include <stdlib.h>


void freeOTL(OTL *otl) {
	freeMTC(otl->mtc);
	free(otl);
}

void freeTL(struct TASKLIST *tl) {
	if (!tl) return;
	int i;
	for (i = 0; i < tl->num; ++i) {
		freeOTL(tl->core[i]);
	}
	free(tl->core);
	free(tl);
}

static void massTL(struct OPTION *op, struct TASKLIST *tl) {
	OTL *otl = smalloc(sizeof(OTL));
	otl->alg = mass;
	otl->train = tl->train;
	otl->test = tl->test;
	otl->trainr_cosine_similarity = tl->trainr_cosine_similarity;
	otl->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
	tl->core[tl->num++] = otl;
	otl->mtc = otl->alg(otl);
}

static void fullTL(struct OPTION *op, struct TASKLIST *tl) {
	struct LineFile *lf = createLF(op->filename_full, INT, INT, -1);
	BIP *left = createBIP(lf, LEFT);
	BIP *right = createBIP(lf, RIGHT);
	freeLF(lf);

	int i;
	for (i = 0; i < op->num_looptimes; ++i) {
		struct LineFile *testf, *trainf;
		divideBIP(left, right, op->rate_dividefulldataset, &testf, &trainf);
		freeBIPS(tl->train);
		tl->train = createBIPS(trainf);
		freeLF(trainf);
		freeBIPS(tl->test);
		tl->test = createBIPS(testf);
		freeLF(testf);

		struct LineFile *simf = cosineSM(tl->train->core[1], tl->train->core[0]);
		freeNETS(tl->trainr_cosine_similarity);
		tl->trainr_cosine_similarity = createNETS(simf);
		freeLF(simf);

		if (op->alg_mass) massTL(op, tl);
		//heatsTL(op, tl);
	}

	freeBIPS(tl->train);
	freeBIPS(tl->test);
	freeNETS(tl->trainr_cosine_similarity);
	freeBIP(left);
	freeBIP(right);
}

static void ttTL(struct OPTION *op, struct TASKLIST *tl) {
	struct LineFile *trainf = createLF(op->filename_train, INT, INT, -1);
	struct LineFile *testf = createLF(op->filename_test, INT, INT, -1);
	tl->train = createBIPS(trainf);
	tl->test = createBIPS(testf);
	freeLF(trainf);
	freeLF(testf);

	struct LineFile *simf = cosineSM(tl->train->core[1], tl->train->core[0]);
	tl->trainr_cosine_similarity = createNETS(simf);
	freeLF(simf);

	if (op->alg_mass) massTL(op, tl);
}

struct TASKLIST *walkingTL(struct OPTION *op) {
	struct TASKLIST *tl = smalloc(sizeof(struct TASKLIST));
	tl->num = algnumOPTION(op) * op->num_looptimes;	
	tl->core = smalloc(tl->num * sizeof(OTL*));
	tl->train = NULL;
	tl->test = NULL;
	tl->trainr_cosine_similarity = NULL;

	tl->num = 0;

	if (op->filename_full) fullTL(op, tl);
	else ttTL(op, tl);

	LOG(LOG_INFO, "tasklist created.");
	return tl;
}
