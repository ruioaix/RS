#include "similar.h"
#include "log.h"
#include "utils.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

LineFile *cosineSM(BIP *bip, SIDE side) {
	HALFBIP *left = bip->left;
	HALFBIP *right = bip->right;
	if (side == RIGHT) {
		left = bip->right;
		right = bip->left;
	}

	int lmaxId = left->maxId;
	int rmaxId = right->maxId;
	int *ldegree = left->degree;
	int **lrela = left->rela;

	int *sign = scalloc((rmaxId + 1),sizeof(int));

	LineFile *simfile = createLF(NULL);

	int step = 1000000;
	int con = step;
	int *i1 = smalloc(con * sizeof(int));
	int *i2 = smalloc(con * sizeof(int));
	double *d1 = smalloc(con * sizeof(double));

	long linesNum = 0;
	int i ,j, k, num_samechoice;
	double simvl;

	for (i = 0; i < lmaxId; ++i) {
		if (ldegree[i]) {
			for (k = 0; k < ldegree[i]; ++k) {
				sign[lrela[i][k]] = 1;
			}
			for (j = i + 1; j < lmaxId + 1; ++j) {
				if (ldegree[j]) {
					num_samechoice = 0;
					for (k = 0; k < ldegree[j]; ++k) {
						num_samechoice += sign[lrela[j][k]];
					}
					if (num_samechoice) {
						simvl = (double)num_samechoice/sqrt(ldegree[i] * ldegree[j]);
						i1[linesNum] = i;
						i2[linesNum] = j;
						d1[linesNum] = simvl;
						++linesNum;
						if (linesNum == con) {
							con += step;
							i1 = srealloc(i1, con * sizeof(int));
							i2 = srealloc(i2, con * sizeof(int));
							d1 = srealloc(d1, con * sizeof(double));
						}
					}
				}
			}
			for (k = 0; k < ldegree[i]; ++k) {
				sign[lrela[i][k]] = 0;
			}
		}
	}
	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;

	LOG(LOG_INFO, "Cosine similarity calculated based on a left&right BIP pair, return a LineFile, linesNum: %ld.", linesNum);
	return simfile;
}

LineFile *pearsonSM(BIP *bip, SIDE side) {
	HALFBIP *left = bip->left;
	HALFBIP *right = bip->right;
	if (side == RIGHT) {
		left = bip->right;
		right = bip->left;
	}

	int lmaxId = left->maxId;
	int rmaxId = right->maxId;
	int *ldegree = left->degree;
	int **lrela = left->rela;
	double **laler = left->aler;

	double *sign = smalloc((rmaxId + 1) * sizeof(double));

	LineFile *simfile = createLF(NULL);

	int step = 1000000;
	int con = step;
	int *i1 = smalloc(con * sizeof(int));
	int *i2 = smalloc(con * sizeof(int));
	double *d1 = smalloc(con * sizeof(double));

	long linesNum = 0;
	int i ,j, k;
	double simvl;
	for (i = 0; i < rmaxId + 1; ++i) {
		sign[i] = 0;
	}

	for (i = 0; i < lmaxId; ++i) {
		if (ldegree[i]) {
			double  sumx = 0, sumxx = 0;
			for (k = 0; k < ldegree[i]; ++k) {
				double x = laler[i][k];
				sign[lrela[i][k]] = x;
				sumx += x;
				sumxx += x * x;
			}
			for (j = i + 1; j < lmaxId + 1; ++j) {
				if (ldegree[j]) {
					//user i and user j.
					double  sumy = 0, sumyy = 0;
					double  sumxy = 0;
					for (k = 0; k < ldegree[j]; ++k) {
						double x = sign[lrela[j][k]];
						double y = laler[j][k];
						sumy += y;
						sumyy += y * y;
						sumxy += x * y;
					}
					double fenzi = sumxy * rmaxId - sumx * sumy;
					double fenmu1 = sumxx * rmaxId - sumx * sumx;
					double fenmu2 = sumyy * rmaxId - sumy * sumy;;
					if (fenzi < 1E-17) {
						simvl = 0;
						//not need to do anything
					}
					else if (fenmu1 < 1E-17 || fenmu2 < 1E-17) {
						LOG(LOG_FATAL, "fezi: %.17f, fenmu1: %.17f fenmu2: %.17f", fenzi, fenmu1, fenmu2);
					}
					else {
						double fenmu = sqrt(fenmu1) * sqrt(fenmu2);
						simvl = fenzi/fenmu;
						//printf("%d, %d, %.17f\n", i, j, soij);
						i1[linesNum] = i;
						i2[linesNum] = j;
						d1[linesNum] = simvl;
						++linesNum;
						if (linesNum == con) {
							con += 1000000;
							i1 = srealloc(i1, con*sizeof(int));
							i2 = srealloc(i2, con*sizeof(int));
							d1 = srealloc(d1, con*sizeof(double));
						}
					}
				}
			}
		}
	}
	free(sign);

	simfile->linesNum = linesNum;
	simfile->i1 = i1;
	simfile->i2 = i2;
	simfile->d1 = d1;

	LOG(LOG_INFO, "calculate pearson similarity done, linesNum: %ld.", linesNum);
	return simfile;
}
