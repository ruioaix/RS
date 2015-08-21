#include "log.h"
#include "alg_mass.h"
#include "metrics.h"
#include "option.h"
#include "net.h"
#include "bip.h"
#include "utils.h"
#include "similar.h"

void full(OPTION *op) {
	struct LineFile *lf = createLF(op->filename_full, INT, INT, -1);
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
		struct LineFile *simf = cosineSM(train);
		NET *trainr_cosine_similarity = createNET(simf);
		freeLF(simf);

		METRICS *mtc;
		if (op->alg_mass) {
			mtc = mass(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics);
		}
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

	LineFile *simf = cosineSM(train);
	NET *trainr_cosine_similarity = createNET(simf);
	freeLF(simf);

	METRICS *mtc;
	if (op->alg_mass) {
		mtc = mass(train, test, trainr_cosine_similarity, op->num_toprightused2cmptmetrics);
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

	releaselog();
	return 0;
}
