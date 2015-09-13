#include "option.h"
#include "log.h"
#include "random.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void display_usage(void) {
	puts("BNRS - Bipart Network Recommemder System 1.0 (Apr 2015)\n");
	puts("usage: bnrs [OPTION]\n");
	puts("OPTION common:");
	puts("  -h:  help");
	puts("  -g logfilename:");
	puts("       File the log system will output log to");
	puts("  -N don't calculate IL metrics");
	puts("");
	puts("OPTION privated to Algorithm:");
	puts("  -m:  Calculate the result of mass algorithm");
	puts("  -a:  Calculate the result of heats algorithm");
	puts("  -B:  Calculate the result of heats2 algorithm");
	puts("  -H:  Calculate the result of hybrid algorithm");
	puts("  -U:  Calculate the result of UCF algorithm");
	puts("  -I:  Calculate the result of ICF algorithm");
	puts("  -Z:  Calculate the result of ZM algorithm");
	puts("  -M:  Calculate the result of ZMU algorithm");
	puts("  -O:  Calculate the result of ZMUO algorithm");
	puts("  -Q:  Calculate the result of KK algorithm");
	puts("");
	puts("OPTION common to Algorithms:");
	puts("  -i filename:");
	puts("       File containing full dataset");
	puts("       if -i is used, then -T and -t will be ignored.");
	puts("       else, -T and -t both have to be present.");
	puts("  -T filename:");
	puts("       File containing train dataset");
	puts("  -t filename:");
	puts("       File containing test dataset");
	puts("  -u filename:  ");
	puts("       File containing extra attribute of the recommending objects");
	puts("  -d doubleValue:  ");
	puts("       Rate used to divide full dataset to train and test dataset");
	puts("       only valid when -i option is used");
	puts("  -c doubleValue:  ");
	puts("       Rate u used in heats2 alg");
	puts("       only valid when -B option is used");
	puts("  -e doubleValue:  ");
	puts("       Rate i used in heats2 alg");
	puts("       only valid when -B option is used");
	puts("  -f doubleValue:  ");
	puts("       Rate u used in kk alg");
	puts("       only valid when -Q option is used");
	puts("  -j doubleValue:  ");
	puts("       Rate i used in kk alg");
	puts("       only valid when -Q option is used");
	puts("  -y doubleValue:  ");
	puts("       Rate used in hybrid alg");
	puts("       only valid when -H option is used");
	puts("  -z doubleValue:  ");
	puts("       Rate used in zm alg");
	puts("       only valid when -z option is used");
	puts("  -r doubleValue:  ");
	puts("       Rate used in zmu alg");
	puts("       only valid when -M option is used");
	puts("  -o doubleValue:  ");
	puts("       Rate used in zmuo alg");
	puts("       only valid when -O option is used");
	puts("  -l intValue:  ");
	puts("       Number of times which the algorthim calculation need to be performed");
	puts("       in order to get reasonable average result");
	puts("  -L intValue:  ");
	puts("       Number of the recommended objects which will be used to computer metrics");
	puts("  -K intValue:  ");
	puts("       Number of K in UCF, only the nearest K users is used");
	puts("  -s unsignedlongValue: ");
	puts("       Random seed");
	puts("");
	exit(0);
}

static void init_OPTION(struct OPTION *op) {
	op->logfilename = NULL;

	op->alg_mass = false;
	op->alg_heats = false;
	op->alg_heats2 = false;
	op->alg_hybrid = false;
	op->alg_ucf = false;
	op->alg_icf = false;
	op->alg_zmo = false;
	op->alg_zmu = false;
	op->alg_zmuo = false;
	op->alg_kk = false;

	op->noIL = false;

	op->filename_full = NULL;
	op->filename_train = NULL;
	op->filename_test = NULL;
	op->rate_dividefulldataset = 0.1;
	op->rate_huparam = 0.2;
	op->rate_hiparam = 0.2;
	op->rate_kuparam = 0.2;
	op->rate_kiparam = 0.2;
	op->rate_hybridparam = 0.2;
	op->rate_zmparam = 0.2;
	op->rate_zmuparam = 0.2;
	op->rate_zmuoparam = 0.2;
	op->num_looptimes = 1;
	op->num_toprightused2cmptmetrics = 50;
	op->num_randomseed= 1;
	op->num_ucf_knn = 50;
}

void freeOPTION(struct OPTION *op) {
	free(op);
}

static void verify_OPTION(struct OPTION *op);
static void info_OPTION(struct OPTION *op);

struct OPTION *setOPTION(int argc, char **argv) {
	if (argc == 1) display_usage();

	struct OPTION *op = smalloc(sizeof(struct OPTION));
	init_OPTION(op);

	static const char *short_options = "hg:NmaBHUIZMOQi:T:t:u:d:c:e:f:j:y:z:r:o:l:L:s:K:";
	struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"log-file", required_argument, NULL, 'g'},
		{"noIL", no_argument, NULL, 'N'},

		{"alg-mass", no_argument, NULL, 'm'},
		{"alg-heats", no_argument, NULL, 'a'},
		{"alg-heats2", no_argument, NULL, 'B'},
		{"alg-hybrid", no_argument, NULL, 'H'},
		{"alg-UCF", no_argument, NULL, 'U'},
		{"alg-ICF", no_argument, NULL, 'I'},
		{"alg-ZM", no_argument, NULL, 'Z'},
		{"alg-ZMU", no_argument, NULL, 'M'},
		{"alg-ZMUO", no_argument, NULL, 'O'},
		{"alg-KK", no_argument, NULL, 'Q'},

		{"filename-full", required_argument, NULL, 'i'},
		{"filename-train", required_argument, NULL, 'T'},
		{"filename-test", required_argument, NULL, 't'},
		{"filename-leftobjectattr", required_argument, NULL, 'u'},

		{"rate-dividefulldataset", required_argument, NULL, 'd'},
		{"rate-huparam", required_argument, NULL, 'c'},
		{"rate-hiparam", required_argument, NULL, 'e'},
		{"rate-kuparam", required_argument, NULL, 'f'},
		{"rate-kiparam", required_argument, NULL, 'j'},
		{"rate-hybridparam", required_argument, NULL, 'y'},
		{"rate-zmparam", required_argument, NULL, 'z'},
		{"rate-zmuparam", required_argument, NULL, 'r'},
		{"rate-zmuoparam", required_argument, NULL, 'o'},
		{"num-looptimes", required_argument, NULL, 'l'},
		{"num-toprightused2cmptmetrics", required_argument, NULL, 'L'},
		{"num-randomseed", required_argument, NULL, 's'},
		{"num-UCF-KNN", required_argument, NULL, 'K'},

		{0, 0, 0, 0},
	};
	int longIndex = 0;
	int opt;
	do {
		opt = getopt_long(argc, argv, short_options, long_options, &longIndex);
		if (opt == -1) break;
		switch (opt) {
			case 'h':
				display_usage();
				break;
			case 'g':
				op->logfilename = optarg;
				break;
			case 'N':
				op->noIL = true;
				break;
			case 'm':
				op->alg_mass = true;
				break;
			case 'a':
				op->alg_heats = true;
				break;
			case 'B':
				op->alg_heats2 = true;
				break;
			case 'H':
				op->alg_hybrid = true;
				break;
			case 'U':
				op->alg_ucf = true;
				break;
			case 'I':
				op->alg_icf = true;
				break;
			case 'Z':
				op->alg_zmo = true;
				break;
			case 'M':
				op->alg_zmu = true;
				break;
			case 'O':
				op->alg_zmuo = true;
				break;
			case 'Q':
				op->alg_kk = true;
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

			case 'd':
				op->rate_dividefulldataset = strtod(optarg, NULL);
				break;
			case 'c':
				op->rate_huparam = strtod(optarg, NULL);
				break;
			case 'e':
				op->rate_hiparam = strtod(optarg, NULL);
				break;
			case 'f':
				op->rate_kuparam = strtod(optarg, NULL);
				break;
			case 'j':
				op->rate_kiparam = strtod(optarg, NULL);
				break;
			case 'y':
				op->rate_hybridparam = strtod(optarg, NULL);
				break;
			case 'z':
				op->rate_zmparam = strtod(optarg, NULL);
				break;
			case 'r':
				op->rate_zmuparam = strtod(optarg, NULL);
				break;
			case 'o':
				op->rate_zmuoparam = strtod(optarg, NULL);
				break;
			case 'l':
				op->num_looptimes = strtol(optarg, NULL, 10);
				break;
			case 'L':
				op->num_toprightused2cmptmetrics  = strtol(optarg, NULL, 10);
				break;
			case 's':
				op->num_randomseed = strtol(optarg, NULL, 10);
				break;
			case 'K':
				op->num_ucf_knn = strtol(optarg, NULL, 10);
				break;

			case '?':
				break;
			default:
				abort();
		}
	} while (1);

	loginit(op->logfilename, getloglevel());
	setRGseed(op->num_randomseed);

	verify_OPTION(op);
	info_OPTION(op);

	return op;
}

static void verify_OPTION(struct OPTION *op) {
	//algorithms
	if (!( op->alg_mass || op->alg_heats || op->alg_ucf || op->alg_icf || op->alg_hybrid || op->alg_zmo || op->alg_zmu || op->alg_zmuo || op->alg_heats2 || op->alg_kk)) {
		LOG(LOG_FATAL, "no algorithms selected, what do you want to calculate?");
	}

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
		}
		//loopnum
		if (op->num_looptimes < 1) {
			LOG(LOG_FATAL, "Are you sure you want to set the loopNum to %d?", op->num_looptimes);
		}
	}
	else {
		//loopnum
		if (op->num_looptimes != 1) {
			LOG(LOG_WARN, "Sorry, we can only loop for 1 time, not %d times, because you supply train and test dataset.", op->num_looptimes);
		}
		op->num_looptimes = 1;
	}

	//L
	if (op->num_toprightused2cmptmetrics < 1) {
		LOG(LOG_FATAL, "Are you sure you want to set the num of the top right objects which will be used to computer metrics to %d?", op->num_toprightused2cmptmetrics);
	}

}

static void info_OPTION(struct OPTION *op) {
	LOG(LOG_INFO, "Algorithm:");
	//option privated to alg
	LOG(LOG_INFO, "  mass:    %s", trueorfalse(op->alg_mass));
	LOG(LOG_INFO, "  heats:    %s", trueorfalse(op->alg_heats));
	LOG(LOG_INFO, "  heats2:    %s", trueorfalse(op->alg_heats2));
	if (op->alg_heats2) {
		LOG(LOG_INFO, "      heats2 u rate: %f", op->rate_huparam);
		LOG(LOG_INFO, "      heats2 i rate: %f", op->rate_hiparam);
	}
	LOG(LOG_INFO, "  kk:    %s", trueorfalse(op->alg_kk));
	if (op->alg_kk) {
		LOG(LOG_INFO, "      kk u rate: %f", op->rate_kuparam);
		LOG(LOG_INFO, "      kk i rate: %f", op->rate_kiparam);
	}
	LOG(LOG_INFO, "  hybrid:    %s", trueorfalse(op->alg_hybrid));
	if (op->alg_hybrid) {
		LOG(LOG_INFO, "      hybrid rate: %f", op->rate_hybridparam);
	}
	LOG(LOG_INFO, "  zm:    %s", trueorfalse(op->alg_zmo));
	if (op->alg_zmo) {
		LOG(LOG_INFO, "      zm rate: %f", op->rate_zmparam);
	}
	LOG(LOG_INFO, "  zmu:    %s", trueorfalse(op->alg_zmu));
	if (op->alg_zmu) {
		LOG(LOG_INFO, "      zmu rate: %f", op->rate_zmuparam);
	}
	LOG(LOG_INFO, "  zmuo:    %s", trueorfalse(op->alg_zmuo));
	if (op->alg_zmuo) {
		LOG(LOG_INFO, "      zmuo rate: %f", op->rate_zmuoparam);
	}
	LOG(LOG_INFO, "  ucf:    %s", trueorfalse(op->alg_ucf));
	if (op->alg_ucf) {
		LOG(LOG_INFO, "      UCF K: %d", op->num_ucf_knn);
	}
	LOG(LOG_INFO, "  icf:    %s", trueorfalse(op->alg_icf));

	if (op->noIL) {
		LOG(LOG_INFO, "Metrics IL will not be calculated");
	}

	//option common to alg
	if (op->filename_full) {
		LOG(LOG_INFO, "Full dataset: %s", op->filename_full);
		LOG(LOG_INFO, "Divide rate:  %f", op->rate_dividefulldataset);
		LOG(LOG_INFO, "The num of loop times for each algorithm: %d", op->num_looptimes);
	}
	else {
		LOG(LOG_INFO, "Train dataset: %s", op->filename_train);
		LOG(LOG_INFO, "Test dataset:  %s", op->filename_test);
		LOG(LOG_INFO, "The num of loop times for each algorithm: %d", op->num_looptimes);
	}
	LOG(LOG_INFO, "The num of top recommeded right objects: %d", op->num_toprightused2cmptmetrics);
	LOG(LOG_INFO, "The seed of random number generater: %lu", op->num_randomseed);
}
