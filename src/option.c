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
	puts("");
	puts("OPTION with arguments:");
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

static void verify_OPTION(struct OPTION *op) {
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

static void init_OPTION(struct OPTION *op) {
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

struct OPTION *setOPTION(int argc, char **argv) {
	struct OPTION *op = smalloc(sizeof(struct OPTION));
	init_OPTION(op);

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

	verify_OPTION(op);
	return op;
}

int algnumOPTION(struct OPTION *op) {
	return (int)(op->alg_mass) + \
		(int)(op->alg_HNBI) + \
		(int)(op->alg_heats) + \
		(int)(op->alg_hybrid);
}
