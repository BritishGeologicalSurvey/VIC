/******************************************************************************
* calculateGWSS
* function calculates steady state GW flow until stability 
* criterion is reached
*****************************************************************************/
#include "Link_AMBHAS_VIC.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

int calculateGwSS(const gw_global_data_struct *g, const gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, domain_struct *vic_domain){
	float balance;
	int ctime=0;
	int ccol, crow;
	balance = 10000000000.0;
	dmy_struct         *dmy = NULL;//to be passed into vic_image_run
	//while (ctime<1000000){
	while (fabs(balance)>g->GLOBAL_ERROR){

		vic_image_run(&(dmy[0]));

		get_VIC_Data_Into_AMBHAS(d, vic_domain);
			
		balance=calculateGwFlow(g, d, p,ts,0);

		get_AMBHAS_Data_Into_VIC(d, p, vic_domain, 0);
			
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
