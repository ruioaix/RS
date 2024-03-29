#include "log.h"
#include "alg_mass.h"
#include "alg_heats.h"
#include "alg_heats2.h"
#include "alg_kk.h"
#include "alg_hybrid.h"
#include "alg_hybridr.h"
#include "alg_zmo.h"
#include "alg_zmu.h"
#include "alg_zmuo.h"
#include "alg_zmou.h"
#include "alg_icf.h"
#include "alg_ucf.h"
#include "metrics.h"
#include "option.h"
#include "net.h"
#include "bip.h"
#include "utils.h"
#include "similar.h"
#include "sort.h"
#include <string.h>

int *level(BIP *train, SIDE side) {
	HALFBIP *hbip = train->left;
	if (side == RIGHT) hbip = train->right;
	int * l = scalloc(hbip->maxId + 1, sizeof(int));
	int *dd = scalloc(hbip->maxId + 1, sizeof(int));
	memcpy(dd, hbip->degree, (hbip->maxId + 1) * sizeof(int));
	qsort_i_desc(dd, 0, hbip->maxId);
	int l1s = dd[(int)(hbip->maxId * 0.01)];
	int l2s = dd[(int)(hbip->maxId * 0.1)];
	int i;
	for (i = 0; i < hbip->maxId + 1; ++i) {
		int deg = hbip->degree[i];
		if (deg > l1s) {
			l[i] = 1;
		}
		else if (deg > l2s) {
			l[i] = 2;
		}
		else {
			l[i] = 3;
		}
	}
	free(dd);
	return l;
}

double *psimMf(LineFile *psimf, int maxId) {
	long L = (maxId + 1) * (maxId + 1);
	double *psimM = smalloc(L * sizeof(double));
	long i;
	for (i = 0; i < L; ++i) {
		psimM[i] = 0;
	}
	for (i = 0; i < psimf->linesNum; ++i) {
		int x = psimf->i1[i];
		int y = psimf->i2[i];
		double d = psimf->d1[i];
		psimM[x * (maxId + 1) + y] = d;
	}
	return psimM;
}

void full(OPTION *op) {
	struct LineFile *lf = createLF(op->filename_full, INT, INT, DOUBLE, -1);
	BIP *full = createBIP(lf);
	freeLF(lf);

	int i;
	for (i = 0; i < op->num_looptimes; ++i) {
		struct LineFile *testf, *trainf;
		divideBIP(full, op->rate_dividefulldataset, &trainf, &testf);

		//train
		BIP *train = createBIP(trainf);
		freeLF(trainf);

		//test
		BIP *test = createBIP(testf);
		freeLF(testf);

		//similarity
		NET *trainr_cosine_similarity = NULL;
		if (!op->noIL) {
			struct LineFile *simf = cosineSM(train, RIGHT);
			trainr_cosine_similarity = createNET(simf);
			freeLF(simf);
		}

		int *l = level(train, RIGHT);
		if (op->alg_mass) {
			METRICS *mtc = mass(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_heats) {
			METRICS *mtc = heats(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_heats2) {
			METRICS *mtc = heats2(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, l, op->rate_huparam, op->rate_hiparam);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_kk) {
			METRICS *mtc = kk(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, l, op->rate_kuparam, op->rate_kiparam);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_hybrid) {
			METRICS *mtc = hybrid(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, op->rate_hybridparam, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_hybridr) {
			METRICS *mtc = hybridr(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, op->rate_hybridrparam, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_zmo) {
			METRICS *mtc = zmo(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, op->rate_zmoparam, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_zmu) {
			METRICS *mtc = zmu(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, op->rate_zmuparam, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_zmuo) {
			METRICS *mtc = zmuo(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, op->rate_zmuoparam, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_zmou) {
			METRICS *mtc = zmou(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, op->rate_zmouparam, l);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_ucf) {
			LineFile *psimf = pearsonSM(train, LEFT);
			double *psimM = psimMf(psimf, train->left->maxId);	
			freeLF(psimf);
			METRICS *mtc = ucf(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, psimM, op->num_ucf_knn, l);
			free(psimM);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_icf) {
			LineFile *psimf = pearsonSM(train, RIGHT);
			double *psimM = psimMf(psimf, train->right->maxId);	
			freeLF(psimf);
			METRICS *mtc = icf(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, psimM, l);
			free(psimM);
			printMTC(mtc);
			freeMTC(mtc);
		}

		free(l);
		freeBIP(train);
		freeBIP(test);
		freeNET(trainr_cosine_similarity);
	}
	freeBIP(full);
}

void tt(OPTION *op) {
	struct LineFile *trainf = createLF(op->filename_train, INT, INT, -1);
	struct LineFile *testf = createLF(op->filename_test, INT, INT, -1);
	BIP *train = createBIP(trainf);
	BIP *test = createBIP(testf);
	freeLF(trainf);
	freeLF(testf);

	LineFile *simf = cosineSM(train, RIGHT);
	NET *trainr_cosine_similarity = createNET(simf);
	freeLF(simf);

	int *l = level(train, RIGHT);
	if (op->alg_mass) {
		METRICS *mtc = mass(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, l);
		freeMTC(mtc);
	}
}

int main(int argc, char **argv) {
	setloglevel(LOG_INFO);
	struct OPTION *op = setOPTION(argc, argv);

	if (op->filename_full) {
		full(op);
	}
	else 
		tt(op);

	freeOPTION(op);
	releaselog();
	return 0;
}
