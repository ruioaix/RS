#include "alg_mass.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>

static void mass_core(int tid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, double *lsource, double *rsource) {

	int i, j, lid, rid;
	int degree;
	double source;

	//one 

	//two
	memset(lsource, 0, (lmaxId + 1) * sizeof(double));
	for (i = 0; i < ldegree[tid]; ++i) {
		rid = lrela[tid][i];
		degree = rdegree[rid];
		source = 1.0 / degree;
		for (j = 0; j < degree; ++j) {
			lid = rrela[rid][j];
			lsource[lid] += source;
		}
	}
	
	//three
	memset(rsource, 0, (rmaxId + 1) * sizeof(double));
	for (i = 0; i < lmaxId + 1; ++i) {
		if (lsource[i]) {
			degree = ldegree[i];
			source = lsource[i] / degree;
			for (j = 0; j < degree; ++j) {
				rid = lrela[i][j];
				rsource[rid] += source;
			}
		}
	}

	for (j = 0; j < ldegree[tid]; ++j) {
		rsource[lrela[tid][j]] = -1;
	}
}

METRICS *mass(BIP *train, BIP *test, NET *trainr_cosine_similarity, int num_toprightused2cmptmetrics, int *l) {
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
	int **rrela = trainr->rela;

	//3 level, from 2 level
	double *lsource = smalloc((lmaxId + 1)*sizeof(double));
	double *rsource = smalloc((rmaxId + 1)*sizeof(double));
	int *rids = smalloc((rmaxId + 1)*sizeof(int));
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
			//get rsource
			mass_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, lsource, rsource);
			//use rsource, get ridts & rank & topL
			settopLrank(L, rmaxId, rdegree, rsource, rids, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, train, test, &R, &RL, &PL);
			set_PLL_METRICS(i, L, rank, test, l, &PL0, &PL1, &PL2, &PL3);
			set_TLL_METRICS(i, test, l, &TL0, &TL1, &TL2, &TL3);
		}
	}
	free(lsource); free(rsource);
	free(rids);
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
