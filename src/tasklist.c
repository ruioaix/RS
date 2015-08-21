#include "tasklist.h"
#include "random.h"
#include "bip.h"
#include "log.h"
#include "net.h"
#include "linefile.h"
#include "similar.h"
#include "alg_mass.h"
#include <stdlib.h>

void freeTASK(TASK *otl) {
	freeMTC(otl->mtc);
	free(otl);
}

void freeTL(struct TASKLIST *tl) {
	if (!tl) return;

	int i;
	for (i = 0; i < tl->listNum; ++i) {
		freeTASK(tl->list[i]);
	}
	free(tl->list);

	freeBIP(tl->train);
	freeBIP(tl->test);
	freeNET(tl->trainr_cosine_similarity);
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
	tl->list = smalloc(tl->listNum * sizeof(TASK *));
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
	tl->list[tl->currNum++] = otl;

	otl->mtc = otl->alg(otl);
}

static void fullTL(struct OPTION *op, struct TASKLIST *tl) {
	struct LineFile *lf = createLF(op->filename_full, INT, INT, -1);
	BIP *full = createBIP(lf);
	freeLF(lf);

	int i, j;
	for (i = 0; i < op->num_looptimes; ++i) {
		struct LineFile *testf, *trainf;
		divideBIP(full, op->rate_dividefulldataset, &trainf, &testf);

		//train
		freeBIP(tl->train);
		tl->train = createBIP(trainf);
		freeLF(trainf);

		//test
		freeBIP(tl->test);
		tl->test = createBIP(testf);
		freeLF(testf);

		//similarity
		freeNET(tl->trainr_cosine_similarity);
		struct LineFile *simf = cosineSM(tl->train);
		tl->trainr_cosine_similarity = createNET(simf);
		freeLF(simf);

		for (j = 0; j < tl->algsNum; ++j) {
			shoot(tl->algs[j](op), tl);	
		}
	}
	freeBIP(full);
}

static void ttTL(struct OPTION *op, struct TASKLIST *tl) {
	struct LineFile *trainf = createLF(op->filename_train, INT, INT, -1);
	struct LineFile *testf = createLF(op->filename_test, INT, INT, -1);
	tl->train = createBIP(trainf);
	tl->test = createBIP(testf);
	freeLF(trainf);
	freeLF(testf);

	struct LineFile *simf = cosineSM(tl->train);
	tl->trainr_cosine_similarity = createNET(simf);
	freeLF(simf);

	if (op->alg_mass) shoot(massT(op), tl);
}

TASKLIST *walkingTL(OPTION *op) {
	struct TASKLIST *tl = initTL(op);

	LOG(LOG_INFO, "start walking TASKLIST...");
	if (op->filename_full) 
		fullTL(op, tl);
	else 
		ttTL(op, tl);

	return tl;
}
