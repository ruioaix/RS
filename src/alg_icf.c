#include "alg_icf.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>

static void icf_core(int tid, int rmaxId, int *ldegree, int **lrela, double *psimM, int *rids, double *rsource) {
	int i, j, neigh;
	for (j = 0; j < rmaxId + 1; ++j) {
		rids[j] = 0;
		rsource[j] = 0;
	}
	for (j = 0; j < ldegree[tid]; ++j) {
		rids[lrela[tid][j]] = 1;
	}

	for (j = 0; j < rmaxId + 1; ++j) {
		if (rids[j]) continue;
		for (i = 0; i < ldegree[tid]; ++i) {
			//neigh is one selected item.
			neigh = lrela[tid][i];
			rsource[j] += psimM[j * (rmaxId + 1) + neigh];
		}
	}
}

METRICS *icf(BIP *train, BIP *test, NET *trainr_cosine_similarity, int num_toprightused2cmptmetrics, double *psimM) {
	LOG(LOG_INFO, "mass enter");
	//1 level, from task
	HALFBIP *trainl = train->left;
	HALFBIP *trainr = train->right;
	HALFBIP *testl = test->left;
	int L = num_toprightused2cmptmetrics;

	//2 level, from 1 level
	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;

	//3 level, from 2 level
	double *lsource = smalloc((lmaxId + 1)*sizeof(double));
	double *rsource = smalloc((rmaxId + 1)*sizeof(double));
	int *rids = smalloc((rmaxId + 1)*sizeof(int));
	int *rank = smalloc((rmaxId + 1)*sizeof(int));
	int *topL = scalloc(L*(lmaxId + 1), sizeof(int));

	struct METRICS *retn = createMTC();
	double R, RL, PL, HL, IL, NL;
	R=RL=PL=HL=IL=NL=0;

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rsource
			icf_core(i, rmaxId, ldegree, lrela, psimM, rids, rsource); 
			//use rsource, get ridts & rank & topL
			settopLrank(L, rmaxId, rdegree, rsource, rids, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, train, test, &R, &RL, &PL);
		}
	}
	free(lsource); free(rsource);
	free(rids);
	free(rank);

	set_HL_METRICS(L, topL, train, &HL);
	set_IL_METRICS(L, topL, train, trainr_cosine_similarity, &IL);
	set_NL_METRICS(L, topL, train, &NL);
	free(topL);

	R /= test->relaNum;
	RL /= testl->idNum;
	PL /= testl->idNum;

	retn->R = R;
	retn->IL = IL;
	retn->NL = NL;
	retn->HL = HL;
	retn->PL = PL;
	retn->RL = RL;
	return retn;
}
