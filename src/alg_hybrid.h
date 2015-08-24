#ifndef HYBRID_H
#define HYBRID_H 

#include "metrics.h"
METRICS *hybrid(BIP *train, BIP *test, NET *trainr_cosine_similarity, int num_toprightused2cmptmetrics, double rate_hybridparam, int *l);

#endif
