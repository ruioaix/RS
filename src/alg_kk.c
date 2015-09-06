#include "alg_kk.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void kk_core(int lid, int rmaxId, int *ldegree, int *rdegree, int **lrela, double *rvltr, double rate_kuparam, double rate_kiparam) {
	int neigh, i, j;
	//one
	double dku = pow(ldegree[lid], rate_kuparam);
	for (i = 0; i < rmaxId + 1; ++i) {
		double dki = pow(rdegree[i], rate_kiparam);
		rvltr[i] = dku * dki;
	}
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		rvltr[neigh] = 0;
	}
}

METRICS *kk(BIP *train, BIP *test, NET *trainr_cosine_similarity, int num_toprightused2cmptmetrics, int *l, double rate_kuparam, double rate_kiparam) {
	//struct METRICS *hybrid(struct TASK *task) {
	LOG(LOG_INFO, "kk enter");
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
	double *lvltr = smalloc((lmaxId + 1)*sizeof(double));
	double *rvltr = smalloc((rmaxId + 1)*sizeof(double));
	int *lidtr = smalloc((lmaxId + 1)*sizeof(int));
	int *ridtr = smalloc((rmaxId + 1)*sizeof(int));
	int *rank = smalloc((rmaxId + 1)*sizeof(int));
	int *topL = scalloc(L*(lmaxId + 1), sizeof(int));

	struct METRICS *retn = createMTC();
	double R, RL, PL, HL, IL, NL;
	R=RL=PL=HL=IL=NL=0;
	double L1, L2, L3, PL0, PL1, PL2, PL3;
	L1=L2=L3=PL0=PL1=PL2=PL3=0;
	double TL0, TL1, TL2, TL3;
	TL0=TL1=TL2=TL3=0;

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rvlts
			kk_core(i, rmaxId, ldegree, rdegree, lrela, rvltr, rate_kuparam, rate_kiparam);
			//use rvlts, get ridts & rank & topL
			int j;
			//set selected item's source to -1
			for (j = 0; j < trainl->degree[i]; ++j) {
				rvltr[trainl->rela[i][j]] = -1;
				//rvlts[uidId[i]] = 0;
			}
			settopLrank(L, rmaxId, rdegree, rvltr, ridtr, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, train, test, &R, &RL, &PL);
			set_PLL_METRICS(i, L, rank, test, l, &PL0, &PL1, &PL2, &PL3);
			set_TLL_METRICS(i, test, l, &TL0, &TL1, &TL2, &TL3);
		}
	}
	free(lvltr); free(rvltr);
	free(lidtr); free(ridtr);
	free(rank);

	set_HL_METRICS(L, topL, train, &HL);
	set_IL_METRICS(L, topL, train, trainr_cosine_similarity, &IL);
	set_NL_METRICS(L, topL, train, &NL);
	set_LL_METRICS(L, topL, train, l, &L1, &L2, &L3);
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

	retn->L1 = L1;
	retn->L2 = L2;
	retn->L3 = L3;
	retn->PL0 = PL0;
	retn->PL1 = PL1;
	retn->PL2 = PL2;
	retn->PL3 = PL3;
	retn->TL0 = TL0;
	retn->TL1 = TL1;
	retn->TL2 = TL2;
	retn->TL3 = TL3;
	return retn;
}
