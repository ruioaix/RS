#include "tasklist.h"
#include "random.h"
#include "bip.h"
#include "log.h"
#include "net.h"
#include "linefile.h"
#include "similar.h"
#include "alg_mass.h"
#include <stdlib.h>

void freeOTL(OTL *otl) {
	freeMTC(otl->mtc);
	free(otl);
}

void freeTL(struct TASKLIST *tl) {
	if (!tl) return;
	int i;
	for (i = 0; i < tl->listNum; ++i) {
		freeOTL(tl->core[i]);
	}

	free(tl->core);
	freeBIPS(tl->train);
	freeBIPS(tl->test);
	freeNET(tl->trainr_cosine_similarity);
	free(tl->dt);
	free(tl->algs);

	free(tl);
}

static struct TASKLIST *initTL(struct OPTION *op) {
	struct TASKLIST *tl = smalloc(sizeof(struct TASKLIST));

	tl->algsNum = algnumOPTION(op);
	tl->algs = smalloc(tl->algsNum * sizeof(ALGS));
	int i = 0;
	if (op->alg_mass) tl->algs[i++] = massT;

	tl->listNum = tl->algsNum * op->num_looptimes;
	tl->core = smalloc(tl->listNum * sizeof(OTL*));
	tl->currNum = 0;
	
	tl->train = NULL;
	tl->test = NULL;
	tl->trainr_cosine_similarity = NULL;
	tl->dt = NULL;

	return tl;
}

static void shoot(struct TASK* otl, struct TASKLIST *tl) {
	otl->train = tl->train;
	otl->test = tl->test;
	otl->trainr_cosine_similarity = tl->trainr_cosine_similarity;
	tl->core[tl->currNum++] = otl;
	otl->dt = tl->dt;

	otl->mtc = otl->alg(otl);
}

static void fullTL(struct OPTION *op, struct TASKLIST *tl) {
	BIPS *full;
	struct LineFile *lf = createLF(op->filename_full, INT, INT, -1);
	full = createBIPS(lf);
	freeLF(lf);

	int i, j;
	for (i = 0; i < op->num_looptimes; ++i) {
		struct LineFile *testf, *trainf;
		divideBIPS(full, op->rate_dividefulldataset, &testf, &trainf);
		//train
		freeBIPS(tl->train);
		tl->train = createBIPS(trainf);
		freeLF(trainf);
		//test
		freeBIPS(tl->test);
		tl->test = createBIPS(testf);
		freeLF(testf);
		//similarity
		freeNET(tl->trainr_cosine_similarity);
		struct LineFile *simf = cosineSM(tl->train->core[1], tl->train->core[0]);
		tl->trainr_cosine_similarity = createNET(simf);
		freeLF(simf);
		for (j = 0; j < tl->algsNum; ++j) {
			shoot(tl->algs[j](op), tl);	
		}
	}


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
	tl->trainr_cosine_similarity = createNET(simf);
	freeLF(simf);

	if (op->alg_mass) shoot(massT(op), tl);
}

struct TASKLIST *walkingTL(struct OPTION *op) {
	struct TASKLIST *tl = initTL(op);

	LOG(LOG_INFO, "start walking TASKLIST...");
	if (op->filename_full) 
		fullTL(op, tl);
	else 
		ttTL(op, tl);

	return tl;
}
