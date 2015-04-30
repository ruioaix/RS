#include "mass.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>

static void mass_core(int lid, double *lts, double *rts, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela) {

	int i, j, neigh;
	int degree;
	double source;

	//one 
	memset(rts, 0, (rmaxId + 1) * sizeof(double));
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		rts[neigh] = 1.0;
	}

	//two
	memset(lts, 0, (lmaxId + 1) * sizeof(double));
	for (i = 0; i < rmaxId + 1; ++i) {
		if (rts[i]) {
			degree = rdegree[i];
			source = rts[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = rrela[i][j];
				lts[neigh] += source;
			}
		}
	}
	
	//three
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		rts[neigh] = 0.0;
	}
	for (i = 0; i < lmaxId + 1; ++i) {
		if (lts[i]) {
			degree = ldegree[i];
			source = (double)lts[i]/(double)degree;
			for (j=0; j<degree; ++j) {
				neigh = lrela[i][j];
				rts[neigh] += source;
			}
		}
	}
}

struct METRICS *mass(struct TASK *task) {
	LOG(LOG_INFO, "alg_mass enter");
	//1 level, from task
	BIP *trainl = task->train->core[0];
	BIP *trainr = task->train->core[1];
	BIP *testl = task->test->core[0];
	int L = task->num_toprightused2cmptmetrics;

	//2 level, from 1 level
	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;
	int **rrela = trainr->rela;

	//3 level, from 2 level
	double *lvlts = smalloc((lmaxId + 1)*sizeof(double));
	double *rvlts = smalloc((rmaxId + 1)*sizeof(double));
	int *lidts= smalloc((lmaxId + 1)*sizeof(int));
	int *ridts = smalloc((rmaxId + 1)*sizeof(int));
	int *rank = smalloc((rmaxId + 1)*sizeof(int));
	int *topL = scalloc(L*(lmaxId + 1), sizeof(int));

	struct METRICS *retn = createMTC();
	double R, RL, PL, HL, IL, NL;
	R=RL=PL=HL=IL=NL=0;

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rvlts
			mass_core(i, lvlts, rvlts, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela);
			//use rvlts, get ridts & rank & topL
			int j;
			//set selected item's source to -1
			for (j = 0; j < trainl->degree[i]; ++j) {
				rvlts[trainl->rela[i][j]] = -1;
				//rvlts[uidId[i]] = 0;
			}
			settopLrank(L, rmaxId, rdegree, rvlts, ridts, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, trainl, trainr, testl, &R, &RL, &PL);
		}
	}
	free(lvlts); free(rvlts);
	free(lidts); free(ridts);
	free(rank);

	set_HL_METRICS(L, topL, trainl, trainr, &HL);
	set_IL_METRICS(L, topL, trainl, trainr, task->trainr_cosine_similarity, &IL);
	set_NL_METRICS(L, topL, trainl, trainr, &NL);
	free(topL);

	R /= testl->edgesNum;
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
