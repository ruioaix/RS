#include "bip.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <limits.h>

#include <string.h>

/*********************************************************************/
/***operation on struct Bip*******************************************/
/*********************************************************************/
static inline void set_maxId_minId_BIP(int *il, long edgesNum, int *maxId, int *minId) {
	//set minId&maxId.
	long i;
	*minId = *maxId = il[0];
	for(i=0; i<edgesNum; ++i) {
		*maxId = *maxId > il[i] ? *maxId : il[i];
		*minId = *minId < il[i] ? *minId : il[i];
	}
}

static inline void set_degree_degreeMax_degreeMin_idNum_BIP(int *il, long edgesNum, int maxId, int *degree, int *degreeMax_retn, int *degreeMin_retn, int *idNum_retn) {
	long i;
	for(i = 0; i < edgesNum; ++i) {
		++degree[il[i]];
	}
	int j, idNum = 0;
	int degreeMax = 0;
	int degreeMin = INT_MAX;
	for(j = 0; j < maxId + 1; ++j) {
		degreeMax = degreeMax>degree[j]?degreeMax:degree[j];
		degreeMin = degreeMin<degree[j]?degreeMin:degree[j];
		if (degree[j]) ++idNum;
	}
	*degreeMax_retn = degreeMax;
	*degreeMin_retn = degreeMin;
	*idNum_retn = idNum;
}

static inline void set_rela_aler_BIP(int *il, int *ii, double *dd, int *degree, int maxId, long edgesNum, int ***rela_retn, double ***aler_retn) {
	if (!ii && !dd) LOG(LOG_FATAL, "ii and dd can not be both NULL");
	if (ii == il) LOG(LOG_FATAL, "il == ii, make noscence");

	int **rela = NULL;
	double **aler = NULL;
	
	if (ii) rela = smalloc((maxId + 1) * sizeof(int *));
	if (dd) aler = smalloc((maxId + 1) * sizeof(double *));
	
	int j;
	if (ii) {
		for(j=0; j<maxId+1; ++j) {
			rela[j]=smalloc(degree[j]*sizeof(int));
		}
	}
	if (dd) {
		for (j = 0; j < maxId + 1; ++j) {
			aler[j] = smalloc(degree[j] * sizeof(double));
		}
	}

	long i;
	int *temp = scalloc(maxId+1, sizeof(int));
	if (ii) {
		for(i=0; i<edgesNum; ++i) {
			int ii1 =il[i];
			rela[ii1][temp[ii1]++]=ii[i];
		}
	}
	memset(temp, 0, (maxId+1)*sizeof(int));
	if (dd) {
		for(i=0; i<edgesNum; ++i) {
			int ii1 =il[i];
			aler[ii1][temp[ii1]++] = dd[i];
		}
	}
	free(temp);

	*rela_retn = rela;
	*aler_retn = aler;
}

static inline HALFBIP *assignHALFBIP(int maxId, int minId, int idNum, int degreeMax, int degreeMin, int *degree, int **rela, double **aler) {
	HALFBIP *hbip = smalloc(sizeof(HALFBIP));
	hbip->maxId=maxId;
	hbip->minId=minId;
	hbip->idNum=idNum;
	hbip->degreeMax = degreeMax;
	hbip->degreeMin = degreeMin;
	hbip->degree = degree;
	hbip->rela = rela;
	hbip->aler = aler;
	return hbip;
}

static HALFBIP* halfBIP(long linesNum, int *il, int *ii, double *dd, int maxId_d, int minId_d, int *degree_d, int degreeMax_d, int degreeMin_d, int idNum_d) {
	int maxId, minId;
	if (maxId_d != -1 && minId_d != -1) {
		maxId = maxId_d;
		minId = minId_d;
	}
	else {
		set_maxId_minId_BIP(il, linesNum, &maxId, &minId);
	}

	int *degree = scalloc(maxId+1, sizeof(int));
	int degreeMax, degreeMin, idNum;
	if (degree_d && degreeMax_d != -1 && degreeMin_d != -1 && idNum_d != -1) {
		memcpy(degree, degree_d, (maxId + 1) * sizeof(int));
		degreeMax = degreeMax_d;
		degreeMin = degreeMin_d;
		idNum = idNum_d;
	}
	else {
		set_degree_degreeMax_degreeMin_idNum_BIP(il, linesNum, maxId, degree, &degreeMax, &degreeMin, &idNum);
	}

	int **rela;
	double **aler;
	set_rela_aler_BIP(il, ii, dd, degree, maxId, linesNum, &rela, &aler);

	return  assignHALFBIP(maxId, minId, idNum, degreeMax, degreeMin, degree, rela, aler);
}

BIP *createBIP(const struct LineFile * const lf) {
	if (lf == NULL || lf->i1 == NULL || lf->linesNum < 1) 
		LOG(LOG_FATAL, "create_Bip lf: %p, lf->i1: %p, lf->i2: %p, lf->linesNum: %ld", lf, lf->i1, lf->i2, lf->linesNum);

	int *i1 = lf->i1;
	int *i2 = lf->i2;
	long edgesNum = lf->linesNum;
	double *d1 = lf->d1;

	BIP *bip = smalloc(sizeof(BIP));
	bip->relaNum = edgesNum;
	bip->left = halfBIP(edgesNum, i1, i2, d1, -1, -1, NULL, -1, -1, -1);
	bip->right = halfBIP(edgesNum, i2, i1, d1, -1, -1, NULL, -1, -1, -1);

	LOG(LOG_INFO, "EdgesNum: %5ld", bip->relaNum);
	LOG(LOG_INFO, "Left:  Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d", bip->left->maxId, bip->left->minId, bip->left->idNum, bip->left->degreeMax, bip->left->degreeMin);
	LOG(LOG_INFO, "Right:  Max: %5d, Min: %5d, Num: %5d, degreeMax: %5d, degreeMin: %5d", bip->right->maxId, bip->right->minId, bip->right->idNum, bip->right->degreeMax, bip->right->degreeMin);
	return bip;
}

static inline void freeHALFBIP(HALFBIP *hbip) {
	int i = 0;
	if (hbip->rela) {
		for(i = 0; i < hbip->maxId + 1; ++i) {
			free(hbip->rela[i]);
		}
	}
	if (hbip->aler) {
		for(i = 0; i < hbip->maxId+1; ++i) {
			free(hbip->aler[i]);
		}
	}
	free(hbip->degree);
	free(hbip->rela);
	free(hbip->aler);
	free(hbip);
}
void freeBIP(BIP *bip) {
	freeHALFBIP(bip->left);
	freeHALFBIP(bip->right);
	free(bip);	
}

static inline HALFBIP *cloneHALFBIP(HALFBIP* old) {
	HALFBIP *new = smalloc(sizeof(HALFBIP));

	new->maxId = old->maxId;
	new->minId = old->minId;
	new->idNum = old->idNum;
	new->degreeMax = old->degreeMax;
	new->degreeMin = old->degreeMin;

	new->degree = smalloc((old->maxId + 1) * sizeof(int));
	memcpy(new->degree, old->degree, (old->maxId + 1) * sizeof(int));

	new->rela = NULL;
	new->aler = NULL;

	int i;
	if (old->rela) {
		new->rela = smalloc((new->maxId + 1)*sizeof(int *));
		for (i = 0; i < new->maxId + 1; ++i) {
			new->rela[i] = smalloc(new->degree[i]*sizeof(int));
			memcpy(new->rela[i], old->rela[i], new->degree[i]*sizeof(int));
		}
	}
	if (old->aler) {
		new->aler = smalloc((new->maxId + 1)*sizeof(double *));
		for (i = 0; i < old->maxId + 1; ++i) {
			new->aler[i] = smalloc(new->degree[i]*sizeof(double));
			memcpy(new->aler[i], old->aler[i], new->degree[i]*sizeof(double));
		}
	}
	return new;
}

BIP* cloneBIP(BIP *old) {
	BIP *new = smalloc(sizeof(BIP));
	new->left = cloneHALFBIP(old->left);
	new->right = cloneHALFBIP(old->right);
	new->relaNum = old->relaNum;
	return new;
}

void verifyBIP(BIP *bip) {
	FILE *fpd = sfopen("/tmp/duplicatePairsinNet", "w");
	FILE *fpc = sfopen("/tmp/NoDuplicatePairsNetFile", "w");
	fprintf(fpd, "the following pairs are duplicate in the net file\n");
	bool sign=false;
	int j, k, i;

	HALFBIP *bl = bip->left;
	HALFBIP *br = bip->right;

	int *place = smalloc((br->maxId+1)*sizeof(int));
	for (k=0; k<br->maxId + 1; ++k) {
		place[k] = -1;
	}
	for (j=0; j<bl->maxId+1; ++j) {
		for (i=0; i<bl->degree[j]; ++i) {
			int rightobj = bl->rela[j][i];
			int holder = place[rightobj];
			if (holder == -1) {
				place[rightobj]= 1;
				fprintf(fpc, "%d\t%d\n", j, rightobj);
				continue;
			}
			fprintf(fpd, "%d\t%d\n", j, rightobj);
			sign=true;
		}
		for (i = 0; i < bl->degree[j]; ++i) {
			place[bl->rela[j][i]] = -1;
		}
	}
	free(place);
	fclose(fpd);
	fclose(fpc);
	if (sign) {
		LOG(LOG_WARN, "the file has duplicate pairs, you can check data/duplicatePairsinNet.");
		LOG(LOG_WARN, "we generate a net file named /tmp/NoDuplicatePairsNetFile which doesn't contain any duplicate pairs.");
		LOG(LOG_WARN, "you should use this file instead the origin wrong one.");
	}
	else {
		LOG(LOG_INFO, "perfect network.");
	}
}

void printBIP(BIP *bip, char *filename) {
	FILE *fp = sfopen(filename, "w");
	int i;
	int j;
	HALFBIP *bl = bip->left;
	for (i = 0; i < bl->maxId + 1; ++i) {
		for (j = 0; j < bl->degree[i]; ++j) {
			fprintf(fp, "%d\t", i);
			if (bl->rela) {
				fprintf(fp, "\t%d", bl->rela[i][j]);
			}
			if (bl->aler) {
				fprintf(fp, "\t%f", bl->aler[i][j]);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	LOG(LOG_INFO, "BIP printed in %s.", filename);
}

void divideBIP(BIP *bip, double rate, struct LineFile **more, struct LineFile **less) {
	if (rate <=0 || rate >= 1) {
		LOG(LOG_FATAL, "divide_Bip error: wrong rate.");
	}
	rate = rate>0.5?1-rate:rate;
	HALFBIP *bl = bip->left;
	HALFBIP *br = bip->right;

	long l1, l2;
	if (bip->relaNum > 100000) {
		l1 = (long)(bip->relaNum * (rate + 0.1));
		l2 = (long)(bip->relaNum * (1-rate + 0.1));
	}
	else {
		l2 = l1 = bip->relaNum;
	}

	*more= createLF(NULL);
	*less = createLF(NULL);

	(*more)->i1 = smalloc(l2*sizeof(int));
	(*less)->i1 = smalloc(l1*sizeof(int));
	
	if (bl->rela) {
		(*less)->i2 = smalloc(l1*sizeof(int));
		(*more)->i2 = smalloc(l2*sizeof(int));
	}
	if (bl->aler) {
		(*less)->d1 = smalloc(l1 * sizeof(double));
		(*more)->d1 = smalloc(l2 * sizeof(double));
	}

	long line1=0, line2=0;

	int *lsign = scalloc(bl->maxId + 1, sizeof(int));
	int *ldegree = smalloc((bl->maxId + 1)*sizeof(int));
	memcpy(ldegree, bl->degree, (bl->maxId + 1)*sizeof(int));

	int *rsign = scalloc(br->maxId + 1, sizeof(int));
	int *rdegree = smalloc((br->maxId + 1)*sizeof(int));
	memcpy(rdegree, br->degree, (br->maxId + 1)*sizeof(int));

	int i, j, neigh;
	for (i=0; i<bl->maxId + 1; ++i) {
		for (j=0; j<bl->degree[i]; ++j) {
			neigh = bl->rela[i][j];

			if (randomd01() < rate) {
				if ((ldegree[i] == 1 && lsign[i] == 0) || (rdegree[neigh] == 1 && rsign[neigh] == 0)) {
					(*more)->i1[line2] = i;
					if (bl->rela) {
						(*more)->i2[line2] = neigh;
					}
					if (bl->aler) {
						(*more)->d1[line2] = bl->aler[i][j];
					}
					--ldegree[i];
					--rdegree[neigh];
					lsign[i] = 1;
					rsign[neigh] = 1;
					++line2;
					continue;
				}
				(*less)->i1[line1] = i;	
				if (bl->rela) {
					(*less)->i2[line1] = neigh;	
				}
				if (bl->aler) {
					(*less)->d1[line1] = bl->aler[i][j];
				}
				--ldegree[i];
				--rdegree[neigh];
				++line1;
				continue;
			}

			(*more)->i1[line2] = i;	
			if (bl->rela) {
				(*more)->i2[line2] = neigh;	
			}
			if (bl->aler) {
				(*more)->d1[line2] = bl->aler[i][j];
			}
			lsign[i] = 1;
			rsign[neigh] = 1;
			--ldegree[i];
			--rdegree[neigh];
			++line2;
		}
	}
	if ((line1 > l1) || (line2 > l2)) {
		LOG(LOG_FATAL, "l1 and l2 two small.");
	}

	free(lsign);
	free(rsign);
	free(ldegree);
	free(rdegree);

	(*less)->linesNum = line1;
	(*more)->linesNum = line2;

	LOG(LOG_INFO, "Divide left&right BIP into big&small LF:");
	LOG(LOG_INFO, "  rate: %.3f, big file's linesNum: %ld, small file's linesNum: %ld.", rate, line2, line1);
}
