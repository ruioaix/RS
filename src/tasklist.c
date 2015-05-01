#include "tasklist.h"
#include "random.h"
#include "bip.h"
#include "log.h"
#include "net.h"
#include "linefile.h"
#include "similar.h"
#include "mass.h"
#include "heats.h"
#include "hybrid.h"
#include "hnbi.h"
#include "masssc.h"
#include "masssct.h"
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

static void tread(struct TASK* otl, struct TASKLIST *tl) {
	otl->train = tl->train;
	otl->test = tl->test;
	otl->trainr_cosine_similarity = tl->trainr_cosine_similarity;
	tl->core[tl->num++] = otl;

	otl->mtc = otl->alg(otl);
}

static void fullTL(struct OPTION *op, struct TASKLIST *tl) {
	struct LineFile *lf = createLF(op->filename_full, INT, INT, INT, -1);
	BIPS *full = createBIPS(lf);
	freeLF(lf);

	int i;
	for (i = 0; i < op->num_looptimes; ++i) {
		struct LineFile *testf, *trainf;
		divideBIPS(full, op->rate_dividefulldataset, &testf, &trainf);
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

		if (op->alg_mass) tread(massT(op), tl);
		if (op->alg_heats) tread(heatsT(op), tl);
		if (op->alg_hybrid) tread(hybridT(op), tl);
		if (op->alg_HNBI) tread(hnbiT(op), tl);
		if (op->alg_masssc) tread(massscT(op), tl);
		if (op->alg_masssct) tread(masssctT(op), tl);
	}

	freeBIPS(tl->train);
	freeBIPS(tl->test);
	freeNETS(tl->trainr_cosine_similarity);

	freeBIPS(full);
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

	if (op->alg_mass) tread(massT(op), tl);
}

static int numTL(struct OPTION *op) {
	return algnumOPTION(op) * op->num_looptimes;	
}

struct TASKLIST *walkingTL(struct OPTION *op) {
	struct TASKLIST *tl = smalloc(sizeof(struct TASKLIST));
	tl->core = smalloc(numTL(op) * sizeof(OTL*));
	//init
	tl->train = NULL;
	tl->test = NULL;
	tl->trainr_cosine_similarity = NULL;
	tl->num = 0;

	LOG(LOG_INFO, "start walking TASKLIST...");
	if (op->filename_full) fullTL(op, tl);
	else ttTL(op, tl);

	return tl;
}
