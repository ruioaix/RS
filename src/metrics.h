#ifndef METRICS_H
#define METRICS_H 

struct METRICS {
	double R;
	double RL;
	double PL;
	double HL;
	double IL;
	double NL;
	double COV;
};

struct METRICS *createMTC(void);
void freeMTC(struct METRICS *m);

#include "bip.h"
void set_R_RL_PL_METRICS(int lid, int L, int *rank, BIP *trainl, BIP *trainr, BIP *testl, double *R, double *RL, double *PL);
#endif
