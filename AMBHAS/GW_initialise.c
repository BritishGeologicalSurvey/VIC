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

int GW_initialise(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts){
	int count; int crow; int ccol; int ctime;
	int debug=1;
	dmy_struct         *dmy = NULL;//to be passed into vic_image_run

	if (debug==1){
		//read observation points and global parameters
		printf("Simulation mode %d \n", g->SIM_MODE);
		printf("OUT_OPTION %d \n" ,g->OUT_OPTION);
		printf("CONFINED %d \n" ,g->CONFINED);




		printf("dem in GW_initialise:\n");
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
			printf(" %.4f ",d->lattitude[crow]);
		}
		printf("\n");
		printf("lons: \n");
		for (ccol=0; ccol<g->NCOL; ccol ++){
			printf(" %.4f ",d->longitude[ccol]);
		}
	}

	getObsDataCount(g, d);
 	if (debug==1){
		printf("getObsDataCount has been called:\n");
	}
	calculateGwInit(g, d, p);
 
 	if (debug==1){
		printf("calculateGWInit has been called:\n");
	}
	
   		if (debug==1){
			printf("K:\n");
			for (crow=0; crow<d->NROW; crow++){
				for (ccol=0; ccol<d->NCOL; ccol ++){
					printf(" %.1f ",d->K[crow][ccol]);
				}
				printf("\n");
			}
			printf("\n"); printf("\n");
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
				if (d->mask[crow][ccol]==1.0){
					p->h[crow][ccol]=d->dem[crow][ccol]-10.0;
				}
			}
		}
	}

	//update Sy
	//if VARIABLE_SY==0, set Sy to Sy of the aquifer
	if(g->VARIABLE_SY==0){
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
		p->Sy[crow][ccol]=d->Sy_aq[crow][ccol];      
	  }
	 }
	}
	//if VARIABLE_SY==1, update Sy for the initialised heads
	if(g->VARIABLE_SY==1){
		printf("VARIABLE_SY \n");
		calculateSy( d, p);

	}  
  	if (debug==1){
        printf("Sy for time step 0 for SY option %d:\n", g->VARIABLE_SY);
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				printf(" %f ",p->Sy[crow][ccol]);
			}
			printf("\n");
		}
		printf("\n");printf("\n");
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
	return 0;
}
