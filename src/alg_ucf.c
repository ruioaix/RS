#include "alg_mass.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>

static void ucf_core(int tid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, int *lids, double **raler, double *psimM, int K, int *rids, double *lsource, double *rsource) {
	int i, j, neigh;

	for (j = 0; j < rmaxId + 1; ++j) {
		rids[j] = 0;
		rsource[j] = 0;
	}
	for (j = 0; j < ldegree[tid]; ++j) {
		rids[lrela[tid][j]] = 1;
	}

	int k;
	for (j = 0; j < rmaxId + 1; ++j) {
		if (rids[j]) continue;
		k = 0;
		for (i = 0; i < rdegree[j]; ++i) {
			neigh = rrela[j][i];
			int score = raler[j][i];
			lsource[k] = psimM[tid * (lmaxId + 1) + neigh];
			lids[k] = score;
			k++;
		}
		if (k) {
			qsort_di_desc(lsource, 0, k-1, lids);
			for (i = 0; i < k && i < K; ++i) {
				rsource[j] += lsource[i] * lids[i];	
			}
		}
	}
}

METRICS *ucf(BIP *train, BIP *test, NET *trainr_pearson_similarity, int num_toprightused2cmptmetrics, double *psimM, int K, int *l) {
	LOG(LOG_INFO, "UCF enter");
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
	int **rrela = trainr->rela;

	//3 level, from 2 level
	double *lsource = smalloc((lmaxId + 1)*sizeof(double));
	double *rsource = smalloc((rmaxId + 1)*sizeof(double));
	int *lids = smalloc((lmaxId + 1)*sizeof(int));
	int *rids = smalloc((rmaxId + 1)*sizeof(int));
	int *rank = smalloc((rmaxId + 1)*sizeof(int));
	int *topL = scalloc(L*(lmaxId + 1), sizeof(int));

	struct METRICS *retn = createMTC();
	double R, RL, PL, HL, IL, NL;
	R=RL=PL=HL=IL=NL=0;
	double L1, L2, L3, PL0, PL1, PL2, PL3;
	L1=L2=L3=PL0=PL1=PL2=PL3=0;

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rsource
			ucf_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, lids, trainr->aler, psimM, K, rids, lsource, rsource);
			//use rsource, get ridts & rank & topL
			settopLrank(L, rmaxId, rdegree, rsource, rids, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, train, test, &R, &RL, &PL);
			set_PLL_METRICS(i, L, rank, test, l, &PL0, &PL1, &PL2, &PL3);
		}
	}
	free(lsource); free(rsource);
	free(lids); free(rids);
	free(rank);

	set_HL_METRICS(L, topL, train, &HL);
	set_IL_METRICS(L, topL, train, trainr_pearson_similarity, &IL);
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
	return retn;
}
