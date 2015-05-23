#include "option.h"
#include "log.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void display_usage(void) {
	puts("BNRS - Bipart Network Recommemder System 1.0 (Apr 2015)\n");
	puts("usage: bnrs [OPTION]\n");
	puts("OPTION:");
	puts("  -h:  help");
	puts("  -m:  Calculate the result of mass algorithm");
	puts("  -e:  Calculate the result of heats algorithm");
	puts("  -H:  Calculate the result of hybrid algorithm");
	puts("  -N:  Calculate the result of HNBI algorithm");
	puts("  -D:  Calculate the result of mass degree algorithm");
	puts("  -S:  Calculate the result of mass score algorithm");
	puts("  -w:  Calculate the result of mass score (only the third step) algorithm");
	puts("");
	puts("OPTION with arguments:");
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
	puts("  --hybrid-rate doubleValue:  ");
	puts("       Rate used in hybrid algorithm.");
	puts("       only valid when -H option is used, otherwize this arg will be ignored.");
	puts("       default: 0.2");
	puts("  --HNBI-rate doubleValue:  ");
	puts("       Rate used in HNBI algorithm.");
	puts("       only valid when -N option is used, otherwize this arg will be ignored.");
	puts("       default: -0.8");
	puts("  --mass-degree-rate doubleValue:  ");
	puts("       Rate used in mass score algorithm.");
	puts("       only valid when -D option is used, otherwize this arg will be ignored.");
	puts("       default: -0.8");
	puts("  --mass-score-rate doubleValue:  ");
	puts("       Rate used in mass score algorithm.");
	puts("       only valid when -S option is used, otherwize this arg will be ignored.");
	puts("       default: -0.8");
	puts("  --mass-score-third-rate doubleValue:  ");
	puts("       Rate used in mass score (only the third step algorithm.");
	puts("       only valid when -w option is used, otherwize this arg will be ignored.");
	puts("       default: -0.8");
	puts("  -l intValue:  ");
	puts("       Number of times which the algorthim calculation need to be performed");
	puts("       in order to get reasonable average result");
	puts("  -L intValue:  ");
	puts("       Number of the recommended objects which will be used to computer metrics");
	puts("  -s unsignedlongValue: ");
	puts("       Random seed");
	puts("");
	puts("OPTION addtional:");
	puts("  -o logfilename:");
	puts("       File the log system will output log to");
	puts("");
	exit(0);
}

static void verify_OPTION(struct OPTION *op) {
	//algorithms
	if (!( op->alg_mass || op->alg_heats || op->alg_hybrid || op->alg_HNBI || op->alg_masssc || op->alg_masssct || op->alg_massd)) {
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

	//hybrid
	if (op->alg_heats && (op->rate_hybridparam < 1E-7 || op->rate_hybridparam > 1.0 + 1E-10) ) {
		LOG(LOG_FATAL, "Are you sure you want to set the hybrid rate to %f?", op->rate_hybridparam);
	}

}

static void init_OPTION(struct OPTION *op) {
	op->alg_mass = false;
	op->alg_heats = false;
	op->alg_hybrid = false;
	op->rate_hybridparam = 0.2;
	op->alg_HNBI = false;
	op->rate_hnbiparam = -0.8;
	op->alg_massd = false;
	op->rate_massdparam = 0.14;
	op->alg_masssc = false;
	op->rate_massscparam = 0.14;
	op->alg_masssct = false;
	op->rate_masssctparam = 0.14;

	op->filename_leftobjectattr = NULL;
	op->filename_full = NULL;
	op->filename_train = NULL;
	op->filename_test = NULL;

	op->rate_dividefulldataset = 0.1;
	op->num_looptimes = 1;
	op->num_toprightused2cmptmetrics = 50;
	op->seed_random = 1;

	op->logfilename = NULL;
}

void freeOPTION(struct OPTION *op) {
	free(op);
}

struct OPTION *setOPTION(int argc, char **argv) {
	struct OPTION *op = smalloc(sizeof(struct OPTION));
	init_OPTION(op);

	static const char *optString = "hmeHNDSwi:T:t:u:d:l:L:s:o:";
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},

		{"alg_mass", no_argument, NULL, 'm'},
		{"alg_heat", no_argument, NULL, 'e'},
		{"alg_hybrid", no_argument, NULL, 'H'},
		{"hybrid-rate", required_argument, NULL, 300},
		{"alg_HNBI", no_argument, NULL, 'N'},
		{"HNBI-rate", required_argument, NULL, 301},
		{"alg_massd", no_argument, NULL, 'D'},
		{"mass-degree-rate", required_argument, NULL, 304},
		{"alg_masssc", no_argument, NULL, 'S'},
		{"mass-score-rate", required_argument, NULL, 302},
		{"alg_masssct", no_argument, NULL, 'w'},
		{"mass-score-third-rate", required_argument, NULL, 303},

		{"filename_full", required_argument, NULL, 'i'},
		{"filename_train", required_argument, NULL, 'T'},
		{"filename_test", required_argument, NULL, 't'},
		{"filename_leftobjectattr", required_argument, NULL, 'u'},

		{"rate_dividefulldataset", required_argument, NULL, 'd'},
		{"num_looptimes", required_argument, NULL, 'l'},
		{"num_toprightused2cmptmetrics", required_argument, NULL, 'L'},
		{"seed_random", required_argument, NULL, 's'},

		{"log-file", required_argument, NULL, 'o'},
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
			case 300:
				op->rate_hybridparam = strtod(optarg, NULL);
				break;
			case 'N':
				op->alg_HNBI = true;
				break;
			case 301:
				op->rate_hnbiparam = strtod(optarg, NULL);
				break;
			case 'D':
				op->alg_massd = true;
				break;
			case 304:
				op->rate_massdparam = strtod(optarg, NULL);
				break;
			case 'S':
				op->alg_masssc = true;
				break;
			case 302:
				op->rate_massscparam = strtod(optarg, NULL);
				break;
			case 'w':
				op->alg_masssct = true;
				break;
			case 303:
				op->rate_masssctparam = strtod(optarg, NULL);
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

			case 'o':
				op->logfilename = optarg;
				break;

			case '?':
				break;
			default:
				abort();
		}
	} while (1);

	loginit(op->logfilename, getloglevel());

	verify_OPTION(op);

	LOG(LOG_INFO, "Algorithm:");
	LOG(LOG_INFO, "  mass:    %s", trueorfalse(op->alg_mass));
	LOG(LOG_INFO, "  heats:   %s", trueorfalse(op->alg_heats));
	LOG(LOG_INFO, "  hybrid:  %s", trueorfalse(op->alg_hybrid));
	LOG(LOG_INFO, "  HNBI:    %s", trueorfalse(op->alg_HNBI));
	LOG(LOG_INFO, "  massd:  %s", trueorfalse(op->alg_massd));
	LOG(LOG_INFO, "  masssc:  %s", trueorfalse(op->alg_masssc));
	LOG(LOG_INFO, "  masssct: %s", trueorfalse(op->alg_masssct));
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
	LOG(LOG_INFO, "The seed of random number generater: %lu", op->seed_random);
	if (op->alg_hybrid) {
		LOG(LOG_INFO, "hybrid rate:  %f", op->rate_hybridparam);
	}
	if (op->alg_HNBI) {
		LOG(LOG_INFO, "HNBI rate:  %f", op->rate_hnbiparam);
	}
	if (op->alg_massd) {
		LOG(LOG_INFO, "massd rate:  %f", op->rate_massdparam);
	}
	if (op->alg_masssc) {
		LOG(LOG_INFO, "masssc rate:  %f", op->rate_massscparam);
	}
	if (op->alg_masssct) {
		LOG(LOG_INFO, "masssct rate:  %f", op->rate_masssctparam);
	}

	return op;
}

int algnumOPTION(struct OPTION *op) {
	return (int)(op->alg_mass) + \
		(int)(op->alg_HNBI) + \
		(int)(op->alg_heats) + \
		(int)(op->alg_hybrid) + \
		(int)(op->alg_massd) + \
		(int)(op->alg_masssc) + \
		(int)(op->alg_masssct);
}
