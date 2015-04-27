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

#endif
