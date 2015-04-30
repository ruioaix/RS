#include "masssc.h"
#include "sort.h"
#include "log.h"
#include "alg.h"
#include <string.h>
#include <stdlib.h>

static void masssc_core(int lid, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela, double *lvltr, double *rvltr) {

	int maxscore = args->maxscore;
	double theta = args->mass_score;

	double *i1sourceA = args->i1sourceA;
	double *i2sourceA = args->i2sourceA;

	int **i1score = args->traini1->score;
	int **i2score = args->traini2->score;

	if (i1score == NULL || i2source == NULL) isError("mass_score_recommend_Bip");
	int *i2id = args->i2id;
	int i, j, neigh;
	long degree;
	double source;
	double totalsource = 0;
	//one 
	memset(i2id, 0, (i2maxId +1)*sizeof(int));
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (j=0; j<i1count[i1]; ++j) {
		neigh = i1ids[i1][j];
		i2id[neigh] = i1score[i1][j];
		i2source[neigh] = pow(i1score[i1][j], theta);
		totalsource += i2source[neigh];	
	}
	for (j=0; j<i1count[i1]; ++j) {
		i2source[neigh] = i2source[neigh]*i1count[i1]/totalsource;
	}
	//two
	memset(i1source, 0, (i1maxId+1)*sizeof(double));
	for (i=0; i<i2maxId + 1; ++i) {
		if (i2source[i]) {
			degree = i2count[i];
			source = i2source[i];
			totalsource = 0;
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				//i1sourceA[neigh] = pow(maxscore - fabs(0-i2id[i]), theta); 
				i1sourceA[neigh] = pow(maxscore - fabs(i2score[i][j]-i2id[i]), theta); 
				totalsource += i1sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i2ids[i][j];
				i1source[neigh] += source*i1sourceA[neigh]/totalsource;
			}
		}
	}
	//printf("xxxo\n");fflush(stdout);

	//three
	memset(i2source, 0, (i2maxId+1)*sizeof(double));
	for (i=0; i<i1maxId + 1; ++i) {
		if (i1source[i]) {
			totalsource = 0;
			degree = i1count[i];
			source = i1source[i];
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2sourceA[neigh] = pow((double)i1score[i][j]/(double)i2count[neigh], theta);
				totalsource += i2sourceA[neigh];
			}
			for (j=0; j<degree; ++j) {
				neigh = i1ids[i][j];
				i2source[neigh] += source*i2sourceA[neigh]/totalsource;
			}
		}
	}
}

struct METRICS *masssc(struct TASK *task) {
	LOG(LOG_INFO, "mass enter");
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
	double *lvltr = smalloc((lmaxId + 1)*sizeof(double));
	double *rvltr = smalloc((rmaxId + 1)*sizeof(double));
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
			masssc_core(i, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela, lvltr, rvltr);
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

struct TASK *massscT(struct OPTION *op) {
	struct TASK *otl = smalloc(sizeof(struct TASK));
	otl->train = NULL;
	otl->test = NULL;
	otl->trainr_cosine_similarity = NULL;

	otl->alg = masssc;
	otl->num_toprightused2cmptmetrics = op->num_toprightused2cmptmetrics;

	return otl;
}
