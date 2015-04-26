#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <stdbool.h>

#include "log.h"
#include "random.h"
#include "utils.h"

static void display_usage(void) {
	puts("BNRS - Bipart Network Recommemder System 1.0 (Apr 2015)\n");
	puts("usage: bnrs [OPTIONS]\n");
	puts("OPTIONS:");
	puts("  -h:  help");
	puts("  -m:  Calculate the result of mass algorithm");
	puts("  -e:  Calculate the result of heats algorithm");
	puts("  -H:  Calculate the result of hybrid algorithm");
	puts("  -N:  Calculate the result of HNBI algorithm");
	puts("");
	puts("OPTIONS with arguments:");
	puts("  -i filename:");
	puts("       File containing full dataset");
	puts("       if -i is used, then -T and -t will be ignored.");
	puts("       else, -T and -t both have to be present.");
	puts("");
	puts("  -T filename:");
	puts("       File containing train dataset");
	puts("");
	puts("  -t filename:");
	puts("       File containing test dataset");
	puts("");
	puts("  -u filename:  ");
	puts("       File containing extra attribute of the recommending objects");
	puts("");
	puts("  -d doubleValue:  ");
	puts("       Rate used to divide full dataset to train and test dataset");
	puts("       only valid when -i option is used");
	puts("");
	puts("  -l intValue:  ");
	puts("       Number of times which the algorthim calculation need to be performed");
	puts("       in order to get reasonable average result");
	puts("");
	puts("  -L intValue:  ");
	puts("       Number of the recommended objects which will be used to computer metrics");
	puts("");
	puts("  -s unsignedlongValue: ");
	puts("       Random seed");
	puts("");
	exit(0);
}

struct OPTIONS {
	bool alg_mass;
	bool alg_heats;
	bool alg_hybrid;
	bool alg_HNBI;

	char *filename_full;
	char *filename_train;
	char *filename_test;
	char *filename_leftobjectattr;

	double rate_dividefulldataset;
	int num_looptimes;
	int num_toprightused2cmptmetrics;
	unsigned long seed_random;
};

static void init_OPTIONS(struct OPTIONS *op) {
	op->alg_mass = false;
	op->alg_heats = false;
	op->alg_hybrid = false;
	op->alg_HNBI = false;

	op->filename_leftobjectattr = NULL;
	op->filename_full = NULL;
	op->filename_train = NULL;
	op->filename_test = NULL;

	op->rate_dividefulldataset = 0.1;
	op->num_looptimes = 1;
	op->num_toprightused2cmptmetrics = 50;
	op->seed_random = 1;
}

static void set_OPTOINS(int argc, char **argv, struct OPTIONS *op) {
	static const char *optString = "hmeHNi:T:t:u:d:l:L:s:";
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},

		{"alg_mass", no_argument, NULL, 'm'},
		{"alg_heat", no_argument, NULL, 'e'},
		{"alg_hybrid", no_argument, NULL, 'H'},
		{"alg_HNBI", no_argument, NULL, 'N'},

		{"filename_full", required_argument, NULL, 'i'},
		{"filename_train", required_argument, NULL, 'T'},
		{"filename_test", required_argument, NULL, 't'},
		{"filename_leftobjectattr", required_argument, NULL, 'u'},

		{"rate_dividefulldataset", required_argument, NULL, 'd'},
		{"num_looptimes", required_argument, NULL, 'l'},
		{"num_toprightused2cmptmetrics", required_argument, NULL, 'L'},
		{"seed_random", required_argument, NULL, 's'},
		{0, 0, 0, 0},
	};
	int longIndex = 0;
	int opt;
	do {
		opt = getopt_long(argc, argv, optString, longOpts, &longIndex);
		if (opt == -1) break;
		switch (opt) {
			case 'h':
				display_usage();
				exit(0);
				break;
			case 'm':
				op->alg_mass = true;
				break;
			case 'e':
				op->alg_heats = true;
				break;
			case 'H':
				op->alg_hybrid = true;
				break;
			case 'N':
				op->alg_HNBI = true;
				break;
			case 'i':
				op->filename_full = optarg;
				break;
			case 'T':
				op->filename_train = optarg;
				break;
			case 't':
				op->filename_test = optarg;
				break;
			case 'u':
				op->filename_leftobjectattr = optarg;
				break;

			case 'd':
				op->rate_dividefulldataset = strtod(optarg, NULL);
				break;
			case 'l':
				op->num_looptimes = strtol(optarg, NULL, 10);
				break;
			case 'L':
				op->num_toprightused2cmptmetrics  = strtol(optarg, NULL, 10);
				break;
			case 's':
				op->seed_random = strtol(optarg, NULL, 10);
				break;

			case '?':
				break;
			default:
				abort();
		}
	} while (1);
}

static void verify_OPTIONS(struct OPTIONS *op) {
	//algorithms
	if (!( op->alg_mass || op->alg_heats || op->alg_hybrid || op->alg_HNBI )) {
		LOG(LOG_FATAL, "no algorithms selected, what do you want to calculate?");
	}
	LOG(LOG_INFO, "Algorithm:");
	LOG(LOG_INFO, "  mass:   %s", trueorfalse(op->alg_mass));
	LOG(LOG_INFO, "  heats:  %s", trueorfalse(op->alg_heats));
	LOG(LOG_INFO, "  hybrid: %s", trueorfalse(op->alg_hybrid));
	LOG(LOG_INFO, "  HNBI:   %s", trueorfalse(op->alg_HNBI));

	//dataset
	if (op->filename_full == NULL && (op->filename_train == NULL || op->filename_test == NULL)) {
		LOG(LOG_FATAL, "Dataset not enough, what do you want to calculate?");
	}
	
	//rate_dividefulldataset
	if (op->filename_full) {
		if (op->rate_dividefulldataset <= 0 || op->rate_dividefulldataset >= 1) {
			LOG(LOG_FATAL, "We will use the whole dataset file: %s, but why the divide_rate is %f?", op->filename_full , op->rate_dividefulldataset);
		}
		else {
		   op->filename_train = NULL;
		   op->filename_test = NULL;
		   LOG(LOG_INFO, "Full dataset: %s", op->filename_full);
		   LOG(LOG_INFO, "Divide rate:  %f", op->rate_dividefulldataset);
		}
		//loopnum
		if (op->num_looptimes < 1) {
			LOG(LOG_FATAL, "Are you sure you want to set the loopNum to %d?", op->num_looptimes);
		}
		LOG(LOG_INFO, "The num of loop times for each algorithm: %d", op->num_looptimes);
	}
	else {
		LOG(LOG_INFO, "Train dataset: %s", op->filename_train);
		LOG(LOG_INFO, "Test dataset:  %s", op->filename_test);
		//loopnum
		if (op->num_looptimes != 1) {
			LOG(LOG_FATAL, "Sorry, we can only loop for 1 time, not %d times, because you supply train and test dataset.", op->num_looptimes);
		}
		op->num_looptimes = 1;
		LOG(LOG_INFO, "The num of loop times for each algorithm: %d", op->num_looptimes);
	}

	//L
	if (op->num_toprightused2cmptmetrics < 1) {
		LOG(LOG_FATAL, "Are you sure you want to set the num of the top right objects which will be used to computer metrics to %d?", op->num_toprightused2cmptmetrics);
	}
	LOG(LOG_INFO, "The num of top recommeded right objects: %d", op->num_toprightused2cmptmetrics);

	//random seed
	LOG(LOG_INFO, "The seed of random number generater: %lu", op->seed_random);
}

int loglevel = LOG_DBG;
FILE *logstream;

int main(int argc, char **argv) {
	//dbgstream = fopen("/tmp/rdsm.log", "w");
	logstream = stdout;
	struct OPTIONS op;
	init_OPTIONS(&op);
	set_OPTOINS(argc, argv, &op);
	verify_OPTIONS(&op);
	return 0;
}

//
//static void do_work_divide(struct Options *oa);
//static void do_work_merge(void) { }
//
//static void do_work(struct Options *oa) {
//	setRGseed(oa->random_seed);
//	if (oa->total_filename != NULL) {
//		do_work_divide(oa);
//	}
//	else {
//		do_work_merge();
//	}
//}
//
//static void get_ds1_ds2_Bip(struct Options *oa, struct Bip **ds1, struct Bip **ds2) {
//	struct LineFile *lf; 
//	if (oa->calculate_UCF || oa->calculate_ICF || oa->calculate_SVD) {
//		lf = create_LineFile(oa->total_filename, 1, 1, 1, -1);
//	}
//	else {
//		lf = create_LineFile(oa->total_filename, 1, 1, -1);
//	}
//	*ds1 = create_Bip(lf, 1);
//	*ds2 = create_Bip(lf, 2);
//	free_LineFile(lf); 
//}
//
//static void set_att_Bip(struct Bip *tr1, struct Bip *te1) {
//	int i;
//	for (i = 0; i < CA_METRICS_BIP; ++i) {
//		te1->att1[i] = 0;
//		te1->att2[i] = 0;
//	}
//	for (i = 0; i < te1->maxId + 1; ++i) {
//		if (te1->degree[i]) {
//			(te1->att1[tr1->attI1[i]]) ++;
//			te1->att2[tr1->attI2[i]] += te1->degree[i];
//			(te1->att1[tr1->attI1[i]]) ++;
//			te1->att2[tr1->attI2[i]] += te1->degree[i];
//		}
//	}
//	te1->att1[0] = te1->idNum;
//	te1->att2[0] = te1->edgesNum;
//}
//
//static void metrics_add_add(struct Metrics_Bip *sum, struct Metrics_Bip *single) {
//	int i;
//	for (i = 0; i < CA_METRICS_BIP; ++i) {
//		sum->R[i] +=  single->R[i];
//		sum->RL[i] += single->RL[i];
//		sum->PL[i] += single->PL[i];
//		sum->HL[i] += single->HL[i];
//		sum->IL[i] += single->IL[i];
//		sum->NL[i] += single->NL[i];
//		sum->COV[i] += single->COV[i];
//	}
//}
//
//static double *create_psimM(struct LineFile *simf, int maxId) {
//	double *psimM = smalloc((maxId + 1)*(maxId + 1) * sizeof(double));
//	int i;
//	for (i = 0; i <(maxId + 1) * (maxId + 1); ++i) {
//		psimM[i] = 0;	
//	}
//	for (i = 0; i < simf->linesNum; ++i) {
//		psimM[simf->i1[i] * (maxId + 1) + simf->i2[i]] = simf->d1[i];
//		psimM[simf->i2[i] * (maxId + 1) + simf->i1[i]] = simf->d1[i];
//	}
//	return psimM;
//}
//
//static struct Metrics_Bip *calculate_SVD_Bip(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, int L, int F);
//
//static void calculate_Bip(struct Options *oa,
//		struct Bip *tr1, struct Bip *tr2,
//		struct Bip *te1, struct Bip *te2,
//		struct iidNet *trsim, double hybrid_param, double HNBI_param, double RENBI_param,
//		struct Metrics_Bip **mass_result,
//		struct Metrics_Bip **heats_result,
//		struct Metrics_Bip **hybrid_result,
//		struct Metrics_Bip **HNBI_result,
//		struct Metrics_Bip **RENBI_result
//		struct Metrics_Bip **UCF_result,
//		struct Metrics_Bip **ICF_result,
//		struct Metrics_Bip **SVD_result
//		) {
//	if (oa->calculate_mass == 1) {
//		struct Metrics_Bip *one = mass_Bip(tr1, tr2, te1, te2, trsim, oa->L);
//		metrics_add_add(*mass_result, one);
//		free_MetricsBip(one);
//	}
//	if (oa->calculate_heat == 1) {
//		struct Metrics_Bip *one= heats_Bip(tr1, tr2, te1, te2, trsim, oa->L);
//		metrics_add_add(*heats_result, one);
//		free_MetricsBip(one);
//	}
//	if (oa->calculate_hybrid == 1) {
//		struct Metrics_Bip *one = hybrid_Bip(tr1, tr2, te1, te2, trsim, 0.2/*hybrid_param*/, oa->L);
//		metrics_add_add(*hybrid_result, one);
//		free_MetricsBip(one);
//	}
//	if (oa->calculate_HNBI == 1) {
//		struct Metrics_Bip *one= HNBI_Bip(tr1, tr2, te1, te2, trsim, -0.8/*HNBI_param*/, oa->L);
//		metrics_add_add(*HNBI_result, one);
//		free_MetricsBip(one);
//	}
//	if (oa->calculate_RENBI == 1) {
//		struct Metrics_Bip *one = RENBI_Bip(tr1, tr2, te1, te2, trsim, -0.75/*RENBI_param*/, oa->L);
//		metrics_add_add(*RENBI_result, one);
//		free_MetricsBip(one);
//	}
//	if (oa->calculate_UCF == 1) {
//		struct LineFile *simf = pearson_similarity_Bip(tr1, tr2, 1);
//		double *psimM = create_psimM(simf, tr1->maxId);
//		free_LineFile(simf);
//		struct Metrics_Bip *one = UCF_Bip(tr1, tr2, te1, te2, trsim, psimM, oa->L, oa->K);
//		metrics_add_add(*UCF_result, one);
//		free_MetricsBip(one);
//		free(psimM);
//	}
//	if (oa->calculate_ICF == 1) {
//		struct LineFile *simf = pearson_similarity_Bip(tr1, tr2, 2);
//		double *psimM = create_psimM(simf, tr2->maxId);
//		free_LineFile(simf);
//		struct Metrics_Bip *one = ICF_Bip(tr1, tr2, te1, te2, trsim, psimM, oa->L, oa->K);
//		metrics_add_add(*ICF_result, one);
//		free_MetricsBip(one);
//		free(psimM);
//	}
//	if (oa->calculate_SVD == 1) {
//		struct Metrics_Bip *one = calculate_SVD_Bip(tr1, tr2, te1, te2, trsim, oa->L, oa->F);
//		metrics_add_add(*SVD_result, one);
//		free_MetricsBip(one);
//	}
//}
//
//static void print_calculate_result_Bip(struct Options *oa,
//		struct Metrics_Bip *mass_result,
//		struct Metrics_Bip *heats_result,
//		struct Metrics_Bip *hybrid_result,
//		struct Metrics_Bip *HNBI_result,
//		struct Metrics_Bip *RENBI_result,
//		struct Metrics_Bip *UCF_result,
//		struct Metrics_Bip *ICF_result
//		struct Metrics_Bip *SVD_result
//		) {
//	int loopNum = oa->loopNum;
//	int i;
//	if (oa->calculate_mass == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\tmass_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, mass_result->R[i]/loopNum, mass_result->RL[i]/loopNum, mass_result->PL[i]/loopNum, mass_result->IL[i]/loopNum, mass_result->HL[i]/loopNum, mass_result->NL[i]/loopNum, mass_result->COV[i]/loopNum);
//		}
//	}
//	if (oa->calculate_heat == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\theats_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, heats_result->R[i]/loopNum, heats_result->RL[i]/loopNum, heats_result->PL[i]/loopNum, heats_result->IL[i]/loopNum, heats_result->HL[i]/loopNum, heats_result->NL[i]/loopNum, heats_result->COV[i]/loopNum);
//		}
//	}
//	if (oa->calculate_hybrid == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\thybrid_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, hybrid_result->R[i]/loopNum, hybrid_result->RL[i]/loopNum, hybrid_result->PL[i]/loopNum, hybrid_result->IL[i]/loopNum, hybrid_result->HL[i]/loopNum, hybrid_result->NL[i]/loopNum, hybrid_result->COV[i]/loopNum);
//		}
//	}
//	if (oa->calculate_HNBI == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\tHNBI_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, HNBI_result->R[i]/loopNum, HNBI_result->RL[i]/loopNum, HNBI_result->PL[i]/loopNum, HNBI_result->IL[i]/loopNum, HNBI_result->HL[i]/loopNum, HNBI_result->NL[i]/loopNum, HNBI_result->COV[i]/loopNum);
//		}
//	}
//	if (oa->calculate_RENBI == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\tRENBI_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, RENBI_result->R[i]/loopNum, RENBI_result->RL[i]/loopNum, RENBI_result->PL[i]/loopNum, RENBI_result->IL[i]/loopNum, RENBI_result->HL[i]/loopNum, RENBI_result->NL[i]/loopNum, RENBI_result->COV[i]/loopNum);
//		}
//	}
//	if (oa->calculate_UCF == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\tUCF_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, UCF_result->R[i]/loopNum, UCF_result->RL[i]/loopNum, UCF_result->PL[i]/loopNum, UCF_result->IL[i]/loopNum, UCF_result->HL[i]/loopNum, UCF_result->NL[i]/loopNum, UCF_result->COV[i]/loopNum);
//		}
//	}
//	if (oa->calculate_ICF == 1) {
//		for (i = 0; i < CA_METRICS_BIP; ++i) {
//			printf("\tICF_%d\tloopNum: %d, R: %.17f, RL: %.17f, PL: %.17f, IL: %.17f, HL: %.17f, NL: %.17f, COV: %.17f\n", i, loopNum, ICF_result->R[i]/loopNum, ICF_result->RL[i]/loopNum, ICF_result->PL[i]/loopNum, ICF_result->IL[i]/loopNum, ICF_result->HL[i]/loopNum, ICF_result->NL[i]/loopNum, ICF_result->COV[i]/loopNum);
//		}
//	}
//}
//
//static void do_work_divide(struct Options *oa) {
//	struct Bip *ds1, *ds2, *tr1, *tr2, *te1, *te2;
//	struct LineFile *smlp, *bigp;
//
//	get_ds1_ds2_Bip(oa, &ds1, &ds2);
//	set_attI_Bip(oa->user_extra_att, ds1);
//
//	struct Metrics_Bip *mass_result = create_MetricsBip();
//	struct Metrics_Bip *heats_result = create_MetricsBip();
//	struct Metrics_Bip *hybrid_result = create_MetricsBip();
//	struct Metrics_Bip *HNBI_result = create_MetricsBip();
//	struct Metrics_Bip *RENBI_result = create_MetricsBip();
//	struct Metrics_Bip *UCF_result = create_MetricsBip();
//	struct Metrics_Bip *ICF_result = create_MetricsBip();
//	struct Metrics_Bip *SVD_result = create_MetricsBip();
//
//	int i;
//	for (i = 0; i < oa->loopNum; ++i) {
//		divide_Bip(ds1, ds2, oa->dataset_divide_rate, &smlp, &bigp);
//		te1 = create_Bip(smlp, 1); te2 = create_Bip(smlp, 2);
//		tr1 = create_Bip(bigp, 1); tr2 = create_Bip(bigp, 2);
//		free_LineFile(smlp); free_LineFile(bigp);
//		
//		set_att_Bip(tr1, te1);
//
//		struct LineFile *simf = cosine_similarity_Bip(tr1, tr2, 2);
//		struct iidNet *trsim = create_iidNet(simf);
//		free_LineFile(simf);
//
//		calculate_Bip(oa, tr1, tr2, te1, te2, trsim, 0.2, 0.8, -0.75,
//				&mass_result, &heats_result, &hybrid_result, &HNBI_result, &RENBI_result,
//				&UCF_result, &ICF_result, &SVD_result);
//
//		free_Bip(tr1); free_Bip(tr2);
//		free_Bip(te1); free_Bip(te2);
//		free_iidNet(trsim);
//	}
//
//	print_calculate_result_Bip(oa, mass_result, heats_result, hybrid_result, HNBI_result, RENBI_result, UCF_result, ICF_result, SVD_result);
//
//	free_MetricsBip(mass_result);
//	free_MetricsBip(heats_result);
//	free_MetricsBip(hybrid_result);
//	free_MetricsBip(HNBI_result);
//	free_MetricsBip(RENBI_result);
//	free_MetricsBip(UCF_result);
//	free_MetricsBip(ICF_result);
//	free_MetricsBip(SVD_result);
//	free_Bip(ds1); free_Bip(ds2);
//}
//
//
//
//static struct Metrics_Bip *calculate_SVD_Bip(struct Bip *tr1, struct Bip *tr2, struct Bip *te1, struct Bip *te2, struct iidNet *trsim, int L, int F) {
//	/*
//	//memory allocate
//	double *rui = smalloc((tr1->maxId + 1) * (tr2->maxId + 1) * sizeof(double));
//	double *rui_s = smalloc((tr1->maxId + 1) * (tr2->maxId + 1) * sizeof(double)); 
//	double *eui = smalloc((tr1->maxId + 1) * (tr2->maxId + 1) * sizeof(double)); 
//	//all score avarage 
//	long allscore = 0;
//	int i,j,k;
//	for (j = 0; j < tr1->maxId + 1; ++j) {
//		if (tr1->degree[j]) {	
//			for (k = 0; k < tr1->degree[j]; ++k) {
//				allscore+=tr1->score[j][k];	
//			}
//		}
//	}
//	double avascore = (double)allscore/tr1->edgesNum;
//
//	//bi
//	double *itemscore = smalloc((tr2->maxId + 1)*sizeof(double));
//	double SVD_l2 = 25;
//	for (j = 0; j < tr2->maxId + 1; ++j) {
//		itemscore[j] = 0;
//		for (k = 0; k < tr2->degree[j]; ++k) {
//			itemscore[j] += tr2->score[j][k] - avascore;	
//		}
//		itemscore[j] /= SVD_l2 + tr2->degree[j];
//	}
//
//	//bu
//	double *userscore = smalloc((tr1->maxId + 1) * sizeof(double));
//	double SVD_l3 = 25;
//	for (j = 0; j < tr1->maxId + 1; ++j) {
//		userscore[j] = 0;
//		for (k = 0; k < tr1->degree[j]; ++k) {
//			userscore[j] += tr1->score[j][k] - avascore - itemscore[tr1->edges[j][k]];
//		}
//		userscore[j] /= SVD_l3 + tr1->degree[j];
//	}
//
//	//p, q
//	double *p = smalloc((tr1->maxId + 1)*F*sizeof(double));
//	double *q = smalloc((tr2->maxId + 1)*F*sizeof(double));
//	double sqrtF = sqrt(F);
//	for (i = 0; i < (tr1->maxId + 1)*F; ++i) {
//		p[i] = 0.1*get_d_MTPR()/sqrtF;
//	}
//	for (i = 0; i < (tr2->maxId + 1)*F; ++i) {
//		q[i] = 0.1*get_d_MTPR()/sqrtF;	
//	}
//
//	//loop, 
//	//1 rui, 
//	//2 eui,
//	//3 bu,
//	//4 bi,
//	//5 p,
//	//6 q,
//	double SVD_l = 0.3, SVD_g = 0.02;
//	int Tstep = 100;
//	double slowRate = 0.99;
//	for (i = 0; i < tr1->maxId + 1; ++i) {
//		for (j = 0; j < tr2->maxId + 1; ++j) {
//			rui[i * (tr1->maxId + 1) + j] = avascore + userscore[i] + itemscore[j];
//			for (k = 0; k < F; ++k) {
//				rui[i * (tr1->maxId + 1) + j] += p[i * (tr1->maxId + 1) + k] * q[j * (tr2->maxId + 1) + k];
//			}
//		}
//	}
//	for (i = 0; i < tr1->maxId + 1; ++i) {
//		for (j = 0; j < tr1->degree[i]; ++j) {
//			eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] = tr1->score[i][j] - rui[i * (tr1->maxId + 1) + tr1->edges[i][j]];
//		}
//	}
//	for (i = 0; i < tr1->maxId + 1; ++i) {
//		double usi = userscore[i];
//		for (j = 0; j < tr1->degree[i]; ++j) {
//			userscore[i] = usi + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] - SVD_l * usi);
//		}
//	}
//	for (i = 0; i < tr2->maxId + 1; ++i) {
//		double isi = itemscore[i];
//		for (j = 0; j < tr2->degree[i]; ++j) {
//			itemscore[i] = isi + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] - SVD_l * isi);	
//		}
//	}
//	for (i = 0; i < tr1->maxId + 1; ++i) {
//		for (k = 0; k < F; ++k) {
//			double pik = p[i * (tr1->maxId + 1) + k];
//			for (j = 0; j < tr1->degree[i]; ++j) {
//				p[j * (tr1->maxId + 1) + k] = pik + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] * q[tr1->edges[j][i] * (tr2->maxId + 1) + k] - SVD_l * pik);
//			}
//		}
//	}
//	for (i = 0; i < tr2->maxId + 1; ++i) {
//		for (k = 0; k < F; ++k) {
//			double qik = q[i * (tr2->maxId + 1) + k];
//			for (j = 0; j < tr2->degree[j]; ++j) {
//				q[i * (tr2->maxId + 1) + k] = qik + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] * p[tr2->edges[i][j] * (tr1->maxId + 1) + k] - SVD_l * qik);
//			}
//		}
//	}
//
//	double RMSE_T = 0;
//	for (i = 0; i < tr1->maxId + 1; ++i) {
//		for (j = 0; j < tr1->degree[i]; ++j) {
//			RMSE_T += pow(eui[i * (tr1->maxId + 1) + tr1->edges[i][j]], 2);
//		}
//	}
//	RMSE_T = sqrt(RMSE_T/tr1->edgesNum);
//	memcpy(rui_s, rui, sizeof(double) * (tr1->maxId + 1) * (tr2->maxId + 1));
//
//	do {
//		for (i = 0; i < tr1->maxId + 1; ++i) {
//			for (j = 0; j < tr2->maxId + 1; ++j) {
//				rui[i * (tr1->maxId + 1) + j] = avascore + userscore[i] + itemscore[j];
//				for (k = 0; k < F; ++k) {
//					rui[i * (tr1->maxId + 1) + j] += p[i * (tr1->maxId + 1) + k] * q[j * (tr2->maxId + 1) + k];
//				}
//			}
//		}
//		for (i = 0; i < tr1->maxId + 1; ++i) {
//			for (j = 0; j < tr1->degree[i]; ++j) {
//				eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] = tr1->score[i][j] - rui[i * (tr1->maxId + 1) + tr1->edges[i][j]];
//			}
//		}
//		for (i = 0; i < tr1->maxId + 1; ++i) {
//			double usi = userscore[i];
//			for (j = 0; j < tr1->degree[i]; ++j) {
//				userscore[i] = usi + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] - SVD_l * usi);
//			}
//		}
//		for (i = 0; i < tr2->maxId + 1; ++i) {
//			double isi = itemscore[i];
//			for (j = 0; j < tr2->degree[i]; ++j) {
//				itemscore[i] = isi + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] - SVD_l * isi);	
//			}
//		}
//		for (i = 0; i < tr1->maxId + 1; ++i) {
//			for (k = 0; k < F; ++k) {
//				double pik = p[i * (tr1->maxId + 1) + k];
//				for (j = 0; j < tr1->degree[i]; ++j) {
//					p[j * (tr1->maxId + 1) + k] = pik + SVD_g * (eui[i * (tr1->maxId + 1) + tr1->edges[i][j]] * q[tr1->edges[j][i] * (tr2->maxId + 1) + k] - SVD_l * pik);
//				}
//			}
//		}
//		for (i = 0; i < tr2->maxId + 1; ++i) {
//			for (k = 0; k < F; ++k) {
//				double qik = q[i * (tr2->maxId + 1) + k];
//				for (j = 0; j < tr2->degree[j]; ++j) {
//					q[i * (tr2->maxId + 1) + k] = qik + SVD_g * (eui[tr2->edges[i][j] * (tr1->maxId + 1) + i] * p[tr2->edges[i][j] * (tr1->maxId + 1) + k] - SVD_l * qik);
//				}
//			}
//		}
//		double RMSE = 0;
//		for (i = 0; i < tr1->maxId + 1; ++i) {
//			for (j = 0; j < tr1->degree[i]; ++j) {
//				RMSE += pow(eui[i * (tr1->maxId + 1) + tr1->edges[i][j]], 2);
//			}
//		}
//		RMSE = sqrt(RMSE/tr1->edgesNum); 
//
//		if (RMSE < RMSE_T) {
//			memcpy(rui_s, rui, sizeof(double) * (tr1->maxId + 1) * (tr2->maxId + 1));
//		}
//
//		SVD_g *= slowRate;
//	} while (Tstep--);
//	free(p); free(q);
//	free(userscore); free(itemscore);
//	free(rui); free(eui);
//
//	return SVD_Bip(tr1, tr2, te1, te2, trsim, rui_s, ua, L);
//	*/
//		return NULL;
//}
