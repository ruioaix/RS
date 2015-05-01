#include "alg.h"
#include "sort.h"
#include <string.h>

void settopLrank(int L, int rmaxId, int *rdegree, double *rvlts, int *ridts, int *topL, int *rank) {
	
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
			//valid rvlts and ridts will be in the top j positions of the array.
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
	//2, rvlts[x] = 0, which x isn't selected by left object which has source.
	//3, rvlts[x] = -1, which x has already been selected by i1 in train.
	//4, rvlts[x] = -2, which x is the hole, x isn't selected by anyone.
	//
	//after qsort_di_desc, the id of the item with most source will be in ridts[0];
	qsort_di_desc(rvlts, 0, j-1, ridts);
	//copy the top L itemid into topL.
	memcpy(topL, ridts, L*sizeof(int));
	//get rank;
	for (i=0; i<rmaxId + 1; ++i) {
		rank[ridts[i]] = i+1;
	}
}

