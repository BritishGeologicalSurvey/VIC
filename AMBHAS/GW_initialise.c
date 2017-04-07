#include "Link_AMBHAS_VIC.h"
//#include <vic_def.h>
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
/******************************************************************************
* GW_initialise
* reads in spatial GW data, calculates initial conditions and calculates steady state
* Function to be called before the time loop
*****************************************************************************/

int GW_initialise(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, domain_struct *vic_domain){
	int count; int crow; int ccol; int ctime;
	int debug=0;
	dmy_struct         *dmy = NULL;//to be passed into vic_image_run

	//read observation points and global parameters
	printf("Simulation mode %d \n", g->SIM_MODE);
	printf("OUT_OPTION %d \n" ,g->OUT_OPTION);
	printf("CONFINED %d \n" ,g->CONFINED);



	if (debug==1){
		printf("dem after gw_read:\n");
			for (crow=0; crow<g->NROW; crow++){
				for (ccol=0; ccol<g->NCOL; ccol ++){
					printf(" %.1f ",d->dem[crow][ccol]);
				}
				printf("\n");
			}
		printf("\n");
		printf("\n");

		printf("lats: \n");
		for (crow=0; crow<g->NROW; crow++){
			printf(" %.1f ",d->lattitude[crow]);
		}
		printf("\n");
		printf("lons: \n");
		for (ccol=0; ccol<g->NCOL; ccol ++){
			printf(" %.1f ",d->longitude[ccol]);
		}
	}

		



	getObsDataCount(g, d);


	calculateGwInit(g, d, p);

	

	if (debug==1){
		
	}

	//If option RESTART=1 is specified in gw_global_parameters, then call GW_read_h_ini
	if(g->RESTART==1){
		printf("**restart = 1:\n");
		GW_read_h_ini(g, p);

		if (debug==1){
			printf("Head GW_read_h_ini:\n");
			for (crow=0; crow<d->NROW; crow++){
				for (ccol=0; ccol<d->NCOL; ccol ++){
					printf(" %.1f ",p->h[crow][ccol]);
				}
				printf("\n");
			}
			printf("\n"); printf("\n");
		}
	}
	count=0;

	//If option RESTART=0 set the initial h to DEM-10m
	if(g->RESTART==0){
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				p->h[crow][ccol]=d->dem[crow][ccol]-10.0;
			
			}
			
		}
	}

	
	for(ctime=0;ctime<1;ctime++){

		/** Do VIC run for the first time step **/
      	 	// read forcing data
     		vic_force();

		//stores the initial guess of the water table from AMBHAS into VIC
		get_AMBHAS_Data_Into_VIC(d, p, &vic_domain, ctime);

       		// run vic over the domain
        	vic_image_run(&(dmy[ctime]));

    		//write recharge calculated in VIC into AMBHAS
    		get_VIC_Data_Into_AMBHAS(d, &vic_domain);

		//read pumping data		
		GW_read_Ts(g, d, ctime);

		if (debug==1){
			printf("recharge for time step 0:\n");
			for (crow=0; crow<d->NROW; crow++){
				for (ccol=0; ccol<d->NCOL; ccol ++){
					printf(" %.3f ",d->recharge[crow][ccol]);
				}
				printf("\n");
			}
			printf("\n");printf("\n");
		}

		/*if simulation mode is 0 or 2, then calculate a steady state with the input of the first time step*/
		if(g->SIM_MODE==0||g->SIM_MODE==2){


			calculateGwSS(g, d, p, ts);
		}

		/*if simulation mode is 3 or 4, then calculate a dynamic steady state with the input of the PERIOD time steps*/
		if(g->SIM_MODE==3||g->SIM_MODE==4){

		calculateDynamicGwSS(g, d, p,ts);

		}
		GW_write_output(g, d, p,  ctime);
	}
	if (debug==1){
		printf("Head at end of GW initialise:\n");
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				printf(" %.1f ",p->h[crow][ccol]);
			}
			printf("\n");
		}
		printf("\n"); printf("\n");
	}

}
