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
} METRICS;

METRICS *createMTC(void);
void freeMTC(METRICS *m);
void printMTC(METRICS *m);

void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *train, BIP *test, double *R, double *RL, double *PL);
void set_HL_METRICS(int L, int *alltrianl_topL, BIP *train, double *HL);
void set_IL_METRICS(int L, int *alltrianl_topL, BIP *train, NET *cosin_similarity, double *IL);
void set_NL_METRICS(int L, int *alltrianl_topL, BIP *train, double *NL);
void set_SL_METRICS(int L, int *alltrianl_topL, BIP *train, double *score, double *SL);

#endif
