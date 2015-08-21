#ifndef BIP_H
#define BIP_H

#include "linefile.h"

typedef struct HALFBIP {
	//Basic
	int maxId;
	int minId;
	int idNum;
	int degreeMax;
	int degreeMin;
	int *degree;
	int **rela;
	double **aler;
} HALFBIP;

typedef struct BIP{
	long relaNum;
	HALFBIP *left;
	HALFBIP *right;
} BIP;

BIP *createBIP(const struct LineFile * const file);
void freeBIP(BIP *bip);

BIP * cloneBIP(BIP *bip);
void verifyBIP(BIP *bip);
void printBIP(BIP *bip, char *filename);

//the dividation will guarantee that: 
//	for each available left & right object(degree of this object > 0), there will be at least 1 edge of this object in "more" part.
//so maybe some left or right objects will not exist in the "less" part.
void divideBIP(BIP *bip, double rate, struct LineFile **more, struct LineFile **less);

#endif
