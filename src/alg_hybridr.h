#ifndef HYBRIDR_H
#define HYBRIDR_H 

#include "metrics.h"
METRICS *hybridr(BIP *train, BIP *test, NET *trainr_cosine_similarity, int num_toprightused2cmptmetrics, double rate_hybridparam, int *l);

#endif
