#ifndef CN_IIDNET_H
#define CN_IIDNET_H

#include "core.h"
#include "linefile.h"

typedef struct CORE NET;
void freeNET(NET *net);
NET *createNET(const struct LineFile * const lf);

struct NETS {
	int num;
	enum TYPE *sign;
	NET **nets;
};
void freeNETS(struct NETS *net);
struct NETS *createNETS(const struct LineFile * const file);

#endif
