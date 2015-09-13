#include "alg_zmou.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void zmou_core(int lid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, double rate, double *lvltr, double *rvltr) {
	int i, j, neigh;

	//one
	memset(rvltr, 0, (rmaxId + 1) * sizeof(double));
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		double powl = pow(rdegree[neigh], 1 - rate);
		rvltr[neigh] = 1.0 / powl;
	}

	//two
	memset(lvltr, 0, (lmaxId + 1) * sizeof(double));
	for (i = 0; i < rmaxId + 1; ++i) {
		if (rvltr[i]) {
			for (j=0; j<rdegree[i]; ++j) {
				neigh = rrela[i][j];
				double powl = ldegree[neigh];
				lvltr[neigh] += rvltr[i]/powl;
			}
		}
	}

	//three
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		rvltr[neigh] = 0;
	}
	for ( i= 0; i < lmaxId + 1; ++i) {
		if (lvltr[i]) {
			for (j = 0; j < ldegree[i]; ++j) {
				neigh = lrela[i][j];
				double powl = pow(rdegree[neigh], rate);
				rvltr[neigh] += lvltr[i] / powl;
			}
		}
	}
}

METRICS *zmou(BIP *train, BIP *test, NET *trainr_cosine_similarity, int num_toprightused2cmptmetrics, double rate_zmuoparam, int *l) {
//struct METRICS *hybrid(struct TASK *task) {
	LOG(LOG_INFO, "zmou enter");
	//1 level, from task
	HALFBIP *trainl = train->left;
	HALFBIP *trainr = train->right;
	HALFBIP *testl = test->left;
	int L = num_toprightused2cmptmetrics;
	double rate = rate_zmuoparam;

	//2 level, from 1 level
	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;
	int **rrela = trainr->rela;

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
			zmou_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, rate, lvltr, rvltr);
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
