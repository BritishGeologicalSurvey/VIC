#include <stdlib.h>
#include "GW_global_vars.h"
double ** allocate2DArray( int nrow, int ncol){
	double ** a;
	int crow;
	a=(double**)malloc(nrow*sizeof(double *));
	for(crow=0; crow<nrow;crow++){
		a[crow]=(double*)malloc(ncol*sizeof(double));
	}
return a;
}
