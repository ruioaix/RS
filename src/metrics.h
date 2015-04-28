#ifndef METRICS_H
#define METRICS_H 
#include "bip.h"
#include "net.h"

struct METRICS {
	double R;
	double RL;
	double PL;
	double HL;
	double IL;
	double NL;
};

struct METRICS *createMTC(void);
void freeMTC(struct METRICS *m);

void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *trainl, BIP *trainr, BIP *testl, double *R, double *RL, double *PL);
void set_HL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *HL);
void set_IL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, NETS *cosin_similarity, double *IL);
void set_NL_METRICS(int L, int *alltrianl_topL, BIP *trainl, BIP *trainr, double *NL);
#endif
