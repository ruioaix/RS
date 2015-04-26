#ifndef CORE_H
#define CORE_H 

struct CORE {
	//Basic
	long edgesNum;
	int maxId;
	int minId;
	int idNum;
	int degreeMax;
	int degreeMin;
	int *degree;
	int **rela;
	double **aler;
};

#endif
