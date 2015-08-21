#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "utils.h"
#include "linefile.h"

typedef struct NET{
	long edgesNum;
	int maxId;
	int minId;
	int idNum;
	int degreeMax;
	int degreeMin;
	int *degree;
	int **rela;
	double **aler;

} NET;


void freeNET(NET *net);

NET *createNET(const struct LineFile * const lf);
void printNET(NET *net, char *filename);

#endif
