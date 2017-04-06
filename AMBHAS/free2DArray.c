#include <stdlib.h>
#include "GW_global_vars.h"
void free2DArray(double ** a, int nrow){
	int crow;
	for(crow=0; crow<nrow;crow++){
		free(a[crow]);
	}
	free(a);
}
