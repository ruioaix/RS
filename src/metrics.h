#ifndef METRICS_H
#define METRICS_H 
#include "bip.h"
#include "net.h"

typedef struct METRICS {
	double R;
	double RL;
	double PL;
	double HL;
	double IL;
	double NL;
	double SL;

	//level 1
	double L1;
	double L2;
	double L3;
	double PL0;
	double PL1;
	double PL2;
	double PL3;
	double TL0;
	double TL1;
	double TL2;
	double TL3;
} METRICS;

METRICS *createMTC(void);
void freeMTC(METRICS *m);
void printMTC(METRICS *m);

void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *train, BIP *test, double *R, double *RL, double *PL);
void set_HL_METRICS(int L, int *alltrianl_topL, BIP *train, double *HL);
void set_IL_METRICS(int L, int *alltrianl_topL, BIP *train, NET *cosin_similarity, double *IL);
void set_NL_METRICS(int L, int *alltrianl_topL, BIP *train, double *NL);
void set_SL_METRICS(int L, int *alltrianl_topL, BIP *train, double *score, double *SL);
void set_LL_METRICS(int L, int *alltrianl_topL, BIP *train, int *l, double *L1, double *L2, double *L3);
void set_PLL_METRICS(int lid, int L, int *rank, BIP *test, int *l, double *PL0, double *PL1, double *PL2, double *PL3);
void set_TLL_METRICS(int lid, BIP *test, int *l, double *TL0, double *TL1, double *TL2, double *TL3);

#endif
