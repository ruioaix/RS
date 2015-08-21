#include "net.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <limits.h>
#include <string.h>


void freeNET(NET *net) {
	freeCORE(net);
}

static inline void set_maxid_minid_NET(int *i1, int *i2, long linesNum, int *maxId_retn, int *minId_retn) {
	long i;
	int maxId = i1[0];
	int minId = i1[0];
	for(i=0; i<linesNum; ++i) {
		maxId = maxId > i1[i] ? maxId : i1[i];	
		maxId = maxId > i2[i] ? maxId : i2[i];	
		minId = minId < i1[i] ? minId : i1[i];
		minId = minId < i2[i] ? minId : i2[i];
	}
	*maxId_retn = maxId;
	*minId_retn = minId;
}

static inline void set_degree_degreeMax_degreeMin_idNum_NET(int *i1, int *i2, long linesNum, int maxId, int *degree, int *degreeMax, int *degreeMin, int *idNum_retn) {
	int countMax = 0;
	int countMin = INT_MAX;
	int idNum=0;
	long i;
	for(i = 0; i < linesNum; ++i) {
		if (i1) ++degree[i1[i]];
		if (i2) ++degree[i2[i]];
	}
	int j;
	for(j=0; j<maxId+1; ++j) {
		countMax = countMax > degree[j] ? countMax : degree[j];
		countMin = countMin < degree[j] ? countMin : degree[j];
		if (degree[j]>0) {
			++idNum;
		}
	}

	*degreeMax = countMax;
	*degreeMin = countMin;
	*idNum_retn = idNum;
}

//if objtoi1 != NULL, then objtoi2 must be NULL.
//if objtoi2 != NULL, then objtoi1 must be NULL.
static inline void set_rela_aler_NET(int *i1, int *i2, double *dd, int *degree, int maxId, long linesNum, int ***rela_retn, double ***aler_retn) {
	int i;
	int **rela = rela = smalloc((maxId + 1) * sizeof(int *));
	double **aler = NULL;
	if (dd) aler = smalloc((maxId + 1) * sizeof(double *));

	for(i = 0; i < maxId + 1; ++i) {
		if (degree[i] != 0) {
			rela[i] = smalloc(degree[i] * sizeof(int));
			if (aler) aler[i] = smalloc(degree[i] * sizeof(double));
		}
		else {
			rela[i] = NULL;
			if (aler) aler[i] = NULL;
		}
	}

	int *temp_count = scalloc(maxId + 1, sizeof(int));
	for(i=0; i<linesNum; ++i) {
		int ii1 = i1[i];
		int ii2 = i2[i];
		rela[ii1][temp_count[ii1]] = ii2;
		rela[ii2][temp_count[ii2]] = ii1;
		if (aler) {
			aler[ii1][temp_count[ii1]] = dd[i];
			aler[ii2][temp_count[ii2]] = dd[i];
		}
		++temp_count[ii1];
		++temp_count[ii2];
	}
	free(temp_count);

	*rela_retn = rela;
	*aler_retn = aler;
}

static inline NET *assignNET(int maxId, int minId, long edgesNum, int idNum, int degreeMax, int degreeMin, int *degree, int **rela, double **aler) {
	NET *nc = smalloc(sizeof(NET));
	nc->maxId=maxId;
	nc->minId=minId;
	nc->edgesNum=edgesNum;
	nc->idNum=idNum;
	nc->degreeMax = degreeMax;
	nc->degreeMin = degreeMin;
	nc->degree = degree;
	nc->rela = rela;
	nc->aler = aler;
	return nc;
}

NET *createNET(const struct LineFile * const lf) {
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL) return NULL;

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;

	int maxId, minId;
	set_maxid_minid_NET(i1, i2, linesNum, &maxId, &minId);

	int *degree = scalloc(maxId+1, sizeof(int));
	int degreeMax, degreeMin, idNum;
	set_degree_degreeMax_degreeMin_idNum_NET(i1, i2, linesNum, maxId, degree, &degreeMax, &degreeMin, &idNum);

	int **rela;
	double **aler;
	set_rela_aler_NET(i1, i2, NULL, degree, maxId, linesNum, &rela, &aler);

	LOG(LOG_INFO, "  Max: %5d, Min: %5d, edgesNum: %5ld", maxId, minId, linesNum);
	return assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);
}

void printNET(NET *net, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i, j;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			fprintf(fp, "%d", i);
			if (net->rela) fprintf(fp, "\t%d", net->rela[i][j]);
			if (net->aler) fprintf(fp, "\t%f", net->aler[i][j]);
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	LOG(LOG_INFO, "NET printed into \"%s\".", filename);
}
