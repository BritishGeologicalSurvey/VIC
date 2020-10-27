#include <stdlib.h>
#include "GW_global_vars.h"
int ** allocate2DInt( int nrow, int ncol){
	int ** a;
	int crow;
	a=(int**)malloc(nrow*sizeof(int *));
	for(crow=0; crow<nrow;crow++){
		a[crow]=(int*)malloc(ncol*sizeof(int));
	}
	return a;
}
