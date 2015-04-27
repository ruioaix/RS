#include "alg_mass.h"
#include "task.h"

static struct Metrics_Bip *alg_mass(struct TASK *task) {
	//param from args.
	int i1maxId      = task->traini1->maxId;
	int i2maxId      = args->traini2->maxId;
	int i1idNum      = args->traini1->idNum;
	int *i1degree    = args->traini1->degree;
	int *i2degree    = args->traini2->degree;
	int *user_gender = args->user_gender;
	int *user_age = args->user_age;
	struct iidNet *itemSim = args->itemSim;
	int L = args->L;

	//param from here, will be given to args.
	double *i1source = smalloc((i1maxId + 1)*sizeof(double));
	double *i2source = smalloc((i2maxId + 1)*sizeof(double));
	double *i1sourceA = smalloc((i1maxId + 1)*sizeof(double));
	double *i2sourceA = smalloc((i2maxId + 1)*sizeof(double));
	int *i1id = smalloc((i1maxId + 1)*sizeof(int));
	int *i2id = smalloc((i2maxId + 1)*sizeof(int));
	args->i1source = i1source;
	args->i2source = i2source;
	args->i1sourceA = i1sourceA;
	args->i2sourceA = i2sourceA;
	args->i1id = i1id;
	args->i2id = i2id;

	//the result
	struct Metrics_Bip *retn = create_MetricsBip();
	retn->L = L;
	int *topL = scalloc(L*(i1maxId + 1), sizeof(int));
	retn->topL = topL;
	double *R, *RL, *PL, *HL, *IL, *NL, *COV;
	R=retn->R;
	RL=retn->RL;
	PL=retn->PL;
	HL=retn->HL;
	IL=retn->IL;
	NL=retn->NL;
	COV=retn->COV;

	//only use in this function.
	int *rank = smalloc((i2maxId + 1)*sizeof(int));

	int i;
	for (i = 0; i<i1maxId + 1; ++i) {
		if (i1degree[i]) {//each valid user in trainset.
			args->i1 = i;
			//get i2source
			recommend_core(args);
			//use i2source, get i2id & rank & topL
			Bip_core_common_part(args, i2id, rank, topL + i*L, L);
			metrics_R_RL_PL_Bip(i, i1degree, i2maxId/*i2idNum*/, args->testi1, L, rank, user_gender, user_age, R, RL, PL);
		}
	}
	//print_time();

	metrics_HL_COV_Bip(i1maxId, i1degree, i2maxId, L, topL, user_gender, user_age, HL, COV);
	metrics_IL_Bip(i1maxId, i1degree, i1idNum, i2maxId, L, topL, itemSim, user_gender, user_age, IL);
	metrics_NL_Bip(i1maxId, i1degree, i1idNum, i2degree, L, topL, user_gender, user_age, NL);

	for (i = 0; i < CA_METRICS_BIP; ++i) {
		retn->R[i] = R[i] / args->testset_edge_num[i];
		retn->RL[i] = RL[i] / args->testset_node_num[i];
		retn->PL[i] = PL[i] / args->testset_node_num[i];
		retn->HL[i] = HL[i];
		retn->IL[i] = IL[i] / args->testset_node_num[i];
		retn->NL[i] = NL[i] / args->testset_node_num[i];
		retn->COV[i] = COV[i];
	}

	free(i1source); free(i2source);
	free(i1sourceA); free(i2sourceA);
	free(i1id); free(i2id);
	free(rank);
	return retn;
}
