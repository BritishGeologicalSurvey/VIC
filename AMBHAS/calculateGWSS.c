/******************************************************************************
* calculateGWSS
* function calculates steady state GW flow until stability 
* criterion is reached
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

int calculateGwSS(const gw_global_data_struct *g, const gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts){
	float balance;
	int ctime=0;
	int ccol, crow;
	balance = 10000000000.0;
	//while (ctime<1000000){
	while (fabs(balance)>g->GLOBAL_ERROR){
			
		balance=calculateGwFlow(g, d, p,ts,0);
			
		if(ctime==0){
			writeObsBH(g,ts, 0, 1, 1); //create file for ctime=0
		}
		if(ctime % 1000==0){
			if(ctime>0){
				writeObsBH(g,ts, 0, ctime, 2); //append file for ctime>0
			}

			
		}
		ctime++;
	}


	/*printf("qn at the end of SS simulaiton\n");
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			printf("%.f ",p->qn[crow][ccol]);
		}
		printf("\n");
	}
	printf("\n");*/

return 0;
}
