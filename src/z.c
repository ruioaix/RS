#include "log.h"
#include "alg_mass.h"
#include "alg_ucf.h"
#include "metrics.h"
#include "option.h"
#include "net.h"
#include "bip.h"
#include "utils.h"
#include "similar.h"

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
		struct LineFile *simf = cosineSM(train, RIGHT);
		NET *trainr_cosine_similarity = createNET(simf);
		freeLF(simf);

		if (op->alg_mass) {
			METRICS *mtc = mass(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics);
			printMTC(mtc);
			freeMTC(mtc);
		}
		if (op->alg_ucf) {
			LineFile *psimf = pearsonSM(train, LEFT);
			double *psimM = psimMf(psimf, train->left->maxId);	
			METRICS *mtc = ucf(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics, psimM, op->num_ucf_knn);
			free(psimM);
			printMTC(mtc);
			freeMTC(mtc);
		}

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

	if (op->alg_mass) {
		METRICS *mtc = mass(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics);
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
