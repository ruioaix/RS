#include "alg_mass.h"
#include "sort.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

static void mass_recommend_Bip(int lid, double *lts, double *rts, int lmaxId, int rmaxId, int *ldegree, int *rdegree, int **lrela, int **rrela) {

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
	memset(rts, 0, (rmaxId + 1) * sizeof(double));
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

static inline void Bip_core_common_part(int rmaxId, int *rdegree, double *rvlts, int *ridts, int *topL, int L, int *rank) {
	
	//set ridts and rank.
	int j=0, i;
	double dtmp;
	for (i=0; i<rmaxId + 1; ++i) {
		ridts[i] = i;
		//set unexisted item's source to -2.
		if (!rdegree[i]) {
			rvlts[i] = -2;
		}

		if (rvlts[i] > 0 ) {
			//valid rvlts and ridts will be in the top i positions of the array.
			dtmp = rvlts[j];
			rvlts[j] = rvlts[i];
			rvlts[i] = dtmp;

			ridts[i] = ridts[j];
			ridts[j] = i;

			++j;
		}
	}
	//to this step, rvlts contains four parts: 
	//1, nomral rvlts[x] , which (0, 1].
	//2, rvlts[x] = 0, which x isn't selected by anyone has similarity.
	//3, rvlts[x] = -1, which x has been selected by i1.
	//4, rvlts[x] = -2, which x is the hole, x isn't selected by anyone.
	//
	//after qsort_di_desc, the id of the item with most source will be in ridts[0];
	//qsort_di_desc(rvlts, 0, i2maxId, ridts);
	//printf("%d\t%d\n", j, i2maxId);
	qsort_di_desc(rvlts, 0, j-1, ridts);
	//copy the top L itemid into topL.
	memcpy(topL, ridts, L*sizeof(int));
	//get rank;
	for (i=0; i<rmaxId + 1; ++i) {
		rank[ridts[i]] = i+1;
	}
}

struct METRICS *alg_mass(struct TASK *task) {
	LOG(LOG_INFO, "alg_mass enter");
	//param from args.
	BIP *trainl = task->train->core[0];
	BIP *trainr = task->train->core[1];
	BIP *testl = task->test->core[0];
	
	int L = task->num_toprightused2cmptmetrics;

	int lmaxId = trainl->maxId;
	int rmaxId = trainr->maxId;
	int *ldegree = trainl->degree;
	int *rdegree = trainr->degree;
	int **lrela = trainl->rela;
	int **rrela = trainr->rela;
	//param from here, will be given to args.
	double *lvlts = smalloc((trainl->maxId + 1)*sizeof(double));
	double *rvlts = smalloc((trainr->maxId + 1)*sizeof(double));
	int *lidts= smalloc((trainl->maxId + 1)*sizeof(int));
	int *ridts = smalloc((trainr->maxId + 1)*sizeof(int));

	struct METRICS *retn = createMTC();
	int *topL = scalloc(L*(trainl->maxId + 1), sizeof(int));
	double R, RL, PL, HL, IL, NL;
	R=PL=HL=IL=NL=0;

	//only use in this function.
	int *rank = smalloc((trainr->maxId + 1)*sizeof(int));

	int i;
	for (i = 0; i<trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {//each valid user in trainset.
			//get rvlts
			mass_recommend_Bip(i, lvlts, rvlts, lmaxId, rmaxId, ldegree, rdegree, lrela, rrela);
			//use rvlts, get ridts & rank & topL
			int j;
			//set selected item's source to -1
			for (j = 0; j < trainl->degree[i]; ++j) {
				rvlts[trainl->rela[i][j]] = -1;
				//rvlts[uidId[i]] = 0;
			}
			Bip_core_common_part(rmaxId, rdegree, rvlts, ridts, topL + i * L, L, rank);
			set_R_RL_PL_METRICS(i, L, rank, trainl, trainr, testl, &R, &RL, &PL);
		}
	}

	set_HL_METRICS(L, topL, trainl, trainr, &HL);
	set_IL_METRICS(L, topL, trainl, trainr, task->trainr_cosin_similarity, &IL);
	set_NL_METRICS(L, topL, trainl, trainr, &NL);

	free(lvlts); free(rvlts);
	free(lidts); free(ridts);
	free(rank);
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
