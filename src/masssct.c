#include "masssc.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static void masssct_core(int lid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, int **lscore, double rate, double *lvltr, double *rvltr, double *rsctr) {

	int i, j, neigh, degree;
	double source, totalsource = 0;

	//one 
	memset(rvltr, 0, (rmaxId + 1) * sizeof(double));
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		rvltr[neigh] = 1.0;
	}

	//two
	memset(lvltr, 0, (lmaxId + 1) * sizeof(double));
	for (i = 0; i < rmaxId + 1; ++i) {
		if (rvltr[i]) {
			degree = rdegree[i];
			source = rvltr[i] / degree;
			for (j = 0; j < degree; ++j) {
				neigh = rrela[i][j];
				lvltr[neigh] += source;
			}
		}
	}

	//three
	for (j = 0; j < ldegree[lid]; ++j) {
		neigh = lrela[lid][j];
		rvltr[neigh] = 0.0;
	}
	for (i = 0; i < lmaxId + 1; ++i) {
		if (lvltr[i]) {
			totalsource = 0;
			degree = ldegree[i];
			source = lvltr[i];
			for (j = 0; j < degree; ++j) {
				neigh = lrela[i][j];
				rsctr[neigh] = pow((double)lscore[i][j] / (double)rdegree[neigh], rate);
				totalsource += rsctr[neigh];
			}
			for (j = 0; j < degree; ++j) {
				neigh = lrela[i][j];
				rvltr[neigh] += source * rsctr[neigh] / totalsource;
			}
		}
	}
}

struct METRICS *masssct(struct TASK *task) {
	LOG(LOG_INFO, "masssct enter");
	//1 level, from task
	BIP *trainl = task->train->core[0];
	BIP *trainr = task->train->core[1];
	BIP *trainscorel = task->train->core[2];
	BIP *testl = task->test->core[0];
	int L = task->num_toprightused2cmptmetrics;
	double rate = task->rate_masssctparam;

	//2 level, from 1 level
	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;
	int **rrela = trainr->rela;
	int **lscore = trainscorel->rela;

	//3 level, from 2 level
	double *lvltr = smalloc((lmaxId + 1)*sizeof(double));
	double *rvltr = smalloc((rmaxId + 1)*sizeof(double));
	double *lsctr = smalloc((lmaxId + 1)*sizeof(double));
	double *rsctr = smalloc((rmaxId + 1)*sizeof(double));
	int *lidtr = smalloc((lmaxId + 1)*sizeof(int));
	int *ridtr = smalloc((rmaxId + 1)*sizeof(int));
	int *rank = smalloc((rmaxId + 1)*sizeof(int));
	int *topL = scalloc(L*(lmaxId + 1), sizeof(int));

	struct METRICS *retn = createMTC();
	double R, RL, PL, HL, IL, NL;
	R=RL=PL=HL=IL=NL=0;

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rvlts
			masssct_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, lscore, rate, lvltr, rvltr, rsctr);
			//use rvlts, get ridts & rank & topL
			int j;
			//set selected item's source to -1
			for (j = 0; j < trainl->degree[i]; ++j) {
				rvltr[trainl->rela[i][j]] = -1;
				//rvlts[uidId[i]] = 0;
			}
			settopLrank(L, rmaxId, rdegree, rvltr, ridtr, topL + i * L, rank);
			set_R_RL_PL_METRICS(i, L, rank, trainl, trainr, testl, &R, &RL, &PL);
		}
	}
	free(lvltr); free(rvltr);
	free(lidtr); free(ridtr);
	free(lsctr); free(rsctr);
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

struct TASK *masssctT(struct OPTION *op) {
	struct TASK *otl = smalloc(sizeof(struct TASK));
	otl->train = NULL;
	otl->test = NULL;
	otl->trainr_cosine_similarity = NULL;

	otl->alg = masssct;
	otl->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;
	otl->rate_masssctparam = 0.1;

	return otl;
}
