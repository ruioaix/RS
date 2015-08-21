#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "core.h"
#include "utils.h"
#include "linefile.h"

typedef struct CORE NET;

void freeNET(NET *net);

NET *createNET(const struct LineFile * const lf);
void printNET(NET *net, char *filename);

#endif
