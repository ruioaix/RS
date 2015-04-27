#include "net.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

void freeNET(NET *net) {
	if (!net) return;
	int i=0;
	for(i=0; i<net->maxId+1; ++i) {
		if (net->degree[i]>0) {
			free(net->rela[i]);
			free(net->aler[i]);
		}
	}
	free(net->degree);
	free(net->rela);
	free(net->aler);
	free(net);
}

static void set_maxid_minid_NET(int *i1, int *i2, long linesNum, int *maxId_retn, int *minId_retn) {
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

static void set_degree_idNum_NET(int *i1, int *i2, long linesNum, int maxId, int *degree, int *idNum_retn) {
	long i;
	for(i=0; i<linesNum; ++i) {
		++degree[i1[i]];
		++degree[i2[i]];
	}
	int j;
	int idNum=0;
	for(j=0; j<maxId+1; ++j) {
		if (degree[j]>0) {
			++idNum;
		}
	}
	*idNum_retn = idNum;
}

static void set_degreeMax_degreeMin_rela_aler_NET(int *i1, int *i2, int *ii, double *dd, int *degree, int maxId, long linesNum, int *countMax_retn, int *countMin_retn, int ***rela_retn, double ***aler_retn) {
	int countMax=0;
	int countMin = INT_MAX;
	int i;
	int **rela = NULL;
	if (ii) rela = smalloc((maxId+1)*sizeof(int *));
	double **aler = NULL;
	if (dd) aler = smalloc((maxId+1)*sizeof(double *));

	for(i=0; i<maxId+1; ++i) {
		countMax = countMax > degree[i] ? countMax : degree[i];
		countMin = countMin < degree[i] ? countMin : degree[i];
		if (degree[i] != 0) {
			if (rela) rela[i] = smalloc(degree[i]*sizeof(int));
			if (aler) aler[i] = malloc(degree[i]*sizeof(double));
		}
		else {
			if (rela) rela[i] = NULL;
			if (aler) aler[i] = NULL;
		}
	}

	int *temp_count=scalloc(maxId+1, sizeof(int));
	for(i=0; i<linesNum; ++i) {
		int ii1 = i1[i];
		int ii2 = i2[i];
		if (rela) rela[ii1][temp_count[ii1]]=ii[i];
		if (aler) aler[ii1][temp_count[ii1]]=dd[i];
		++temp_count[ii1];
		if (rela) rela[ii2][temp_count[ii2]]=ii[i];
		if (aler) aler[ii2][temp_count[ii2]]=dd[i];
		++temp_count[ii2];
	}
	free(temp_count);

	*countMax_retn = countMax;
	*countMin_retn = countMin;
	*rela_retn = rela;
	*aler_retn = aler;
}

static NET *assignNET(int maxId, int minId, long edgesNum, int idNum, int degreeMax, int degreeMin, int *degree, int **rela, double **aler) {
	NET *net = smalloc(sizeof(NET));
	net->maxId=maxId;
	net->minId=minId;
	net->edgesNum=edgesNum;
	net->idNum=idNum;
	net->degreeMax = degreeMax;
	net->degreeMin = degreeMin;
	net->degree = degree;
	net->rela = rela;
	net->aler = aler;
	return net;
}

NET *createNET(const struct LineFile * const lf) {
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL) return NULL;

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;
	double *d1 = lf->d1;

	int maxId, minId;
	set_maxid_minid_NET(i1, i2, linesNum, &maxId, &minId);

	int idNum, *degree = scalloc(maxId+1, sizeof(int));
	set_degree_idNum_NET(i1, i2, linesNum, maxId, degree, &idNum);

	int degreeMax, degreeMin, **rela;
   	double **aler;
	set_degreeMax_degreeMin_rela_aler_NET(i1, i2, i2, d1, degree, maxId, linesNum, &degreeMax, &degreeMin, &rela, &aler);

	LOG(LOG_INFO, "NET created, Max: %d, Min: %d, idNum: %d, edgesNum: %ld, degreeMax: %d, degreeMin: %d\n", maxId, minId, idNum, linesNum, degreeMax, degreeMin);
	NET *net = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);
	return net;
}

void freeNETS(struct NETS *nets) {
	if (!nets) return;
	int i;
	for (i = 0; i < nets->num; ++i) {
		freeNET(nets->nets[i]);
	}
	free(nets->nets);
	free(nets);
}

struct NETS *createNETS(const struct LineFile * const lf) {
	if (lf == NULL) return NULL;
	if (lf->i1 == NULL || lf->i2 == NULL) return NULL;

	struct NETS *nets = smalloc(sizeof(struct NETS));
	nets->num = columnsNumLF(lf) - 1;
	nets->sign = smalloc(nets->num * sizeof(enum TYPE));
	nets->nets = smalloc(nets->num * sizeof(NET *));

	long linesNum=lf->linesNum;
	int *i1 = lf->i1;
	int *i2 = lf->i2;

	int maxId, minId;
	set_maxid_minid_NET(i1, i2, linesNum, &maxId, &minId);

	int idNum, *degree = scalloc(maxId+1, sizeof(int));
	set_degree_idNum_NET(i1, i2, linesNum, maxId, degree, &idNum);

	int degreeMax, degreeMin, **rela;
	double **aler;

	int i, j = 0;
	for (i = 1; i < lf->iNum; ++i) {
		int *ii = *(lf->ilist[i]);
		if (ii) {
			set_degreeMax_degreeMin_rela_aler_NET(i1, i2, ii, NULL, degree, maxId, linesNum, &degreeMax, &degreeMin, &rela, &aler);
			nets->nets[j] = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);
			nets->sign[j] = INT;
		}
	}
	for (i = 0; i < lf->dNum; ++i) {
		double *dd = *(lf->dlist[i]);
		if (dd) {
			set_degreeMax_degreeMin_rela_aler_NET(i1, i2, NULL, dd, degree, maxId, linesNum, &degreeMax, &degreeMin, &rela, &aler);
			nets->nets[j] = assignNET(maxId, minId, linesNum, idNum, degreeMax, degreeMin, degree, rela, aler);
			nets->sign[j] = INT;
		}
	}

	LOG(LOG_INFO, "NET created, Max: %d, Min: %d, idNum: %d, edgesNum: %ld, degreeMax: %d, degreeMin: %d\n", maxId, minId, idNum, linesNum, degreeMax, degreeMin);
	return nets;
}
