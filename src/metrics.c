#include "metrics.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

struct METRICS *createMTC(void) {
	struct METRICS *lp = smalloc(sizeof(struct METRICS));
	lp->R = 0;
	lp->RL = 0;
	lp->PL = 0;
	lp->HL = 0;
	lp->IL = 0;
	lp->NL = 0;
	lp->COV = 0;
	return lp;
}

void freeMTC(struct METRICS *lp) {
	free(lp);
}

//R is rankscore.
//PL is precision
//Warning: about unselected_list_length, I use bipii->idNum, not bipii->maxId. 
//	but I believe in linyuan's paper, she use the bipii->maxId. 
//	I think bipii->idNum make more sence.
void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *trainl, BIP *trainr, BIP *testl, double *R, double *RL, double *PL) {
	if (lid<testl->maxId + 1 &&  testl->degree[lid]) {
		int unselected_list_length = trainr->maxId - trainl->degree[lid];
		int sum_objintest_rank_in_all = 0;
		int num_objintest_in_topL = 0;
		int j, id;
		for (j=0; j<testl->degree[lid]; ++j) {
			id = testl->rela[lid][j];
			sum_objintest_rank_in_all += rank[id];
			if (rank[id] <= L) {
				++num_objintest_in_topL;
			}
		}

		*R += (double)sum_objintest_rank_in_all/(double)unselected_list_length;
		*RL += (double)num_objintest_in_topL/(double)(testl->degree[lid]);
		*PL += (double)num_objintest_in_topL/(double)L;
	}
}

void set_HL_COV_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *HL, double *COV) {
	int *sign = scalloc((trainr->maxId + 1), sizeof(int));
	int *sign1 = scalloc((trainr->maxId + 1), sizeof(int));

	int i, j, k;

	for (i = 0; i < trainl->maxId + 1; ++i) {
		if (trainl->degree[i]) {
			memset(sign, 0, (trainr->maxId + 1)*sizeof(int));
			for (k = i*L; k < i*L+L; ++k) {
				sign[alltrianl_topL[k]] = 1;
				sign1[alltrianl_topL[k]] = 1;
			}
			for (j = i + 1; j < trainl->maxId + 1; ++j) {
				if (trainl->degree[j]) {
					int Cij = 0;
					for (k = j*L; k < j*L+L; ++k) {
						if (sign[alltrianl_topL[k]]) {
							++Cij;
						}
					}
					*HL += 1 - ((double)Cij)/(double)L;
					cou++;
				}
			}
		}
	}
	HL[0] /= cou[0];
	for (j = 1; j < ca_metrics_bip; ++j) {
		cov[j] = 0;
		for (i = j*(i2maxId + 1); i < (j+1) * (i2maxId + 1); ++i) {
			if (sign2[i]) {
				cov[j]++;
			}	
		}
		COV[j] = (double)(cov[j])/(i2maxId+1);
		HL[j] /= cou[j];
	}
	free(sign);
	free(sign1);
	free(sign2);
}
