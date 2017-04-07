/******************************************************************************
* calculateGWDynamicSS
* This function calculates a dynamic steady state over the period as specified
* in the gw_global_parameters.dat
*****************************************************************************/
#include "GW_global_vars.h"
#include <vic_def.h>
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

int calculateDynamicGwSS(const gw_global_data_struct *g,  gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts){

//printf("function is called \n");
	float balance;
	int ctime, crow, ccol, count, iter, j ;
	float sumdiff;
	balance = 1.0;
	sumdiff=1.0;
	iter=0;
	dmy_struct         *dmy = NULL;//to be passed into vic_image_run
	//while (iter<6){
	while (sumdiff> g->GLOBAL_ERROR){

		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol++){
				p->hini[crow][ccol]=p->h[crow][ccol];

			}

		}

		for(ctime=0; ctime<g->PERIOD;ctime++){
			GW_read_Ts(g, d, ctime);

			/** Do VIC run for the first time step **/
	      	 	 // read forcing data
	     		 vic_force();

	       		// run vic over the domain
			vic_image_run(&(dmy[ctime]));

			GW_read_Ts(g, d, ctime);

			balance=calculateGwFlow(g,d, p, ts, ctime);

		}
	
		sumdiff=0;
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol++){
				sumdiff+=fabs((d->Sy[crow][ccol]*d->area[crow][ccol]*(p->hini[crow][ccol]-p->h[crow][ccol])));
			}
		}
		
		if(iter==0){
			writeObsBH(g,ts, 0, g->PERIOD, 1);
		}
		if(iter>0){

			writeObsBH(g, ts, iter*g->PERIOD, g->PERIOD, 3);
		}

		iter++;
		printf("sumdiff %.5f \n",sumdiff);
	}

return 0;
}

