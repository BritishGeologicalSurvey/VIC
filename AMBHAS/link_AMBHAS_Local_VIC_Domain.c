/******************************************************************************
* link_AMBHAS_Local_VIC_Domain.c
* find AMBHAS nrow ncol for each VIC cell  
* for local domain
*****************************************************************************/

#include "Link_AMBHAS_VIC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void link_AMBHAS_Local_VIC_Domain(gw_data_struct *d, domain_struct *v, int mpi_rank){
	int crow, ccol, count, i, nvic;
	int debug =0;
	double gridsize;
	count = 0;
	printf("link AMBHAS Local VIC Domain is called:\n");
	location_struct *l;

	//get lat lon from VIC
	l=v->locations;
	nvic=(int) v->ncells_active; //note as ncell_total is 0, ncell_active is used here. This is different than for linking the global domain

	if (debug==1){
		printf("ncells_total in local vic domain %d \n", nvic);
		printf("ncells_active in local vic domain %d \n", (int) v->ncells_active);
		printf("NROW %d \n", d->NROW);
		printf("NCOL %d \n", d->NCOL);
	}



	for (i=0; i< nvic; i++){
		(l+i)->A_nlat=-999;//initialise to -999 
		(l+i)->A_nlon=-999;//initialise to -999
	}	

	gridsize=fabs(d->lattitude[1]-d->lattitude[0]);

	if (debug==1){
		printf("gridsize %f \n",gridsize );
		printf(" lattitude[1] %f, lattitude[0] %f\n", d->lattitude[1], d->lattitude[0] );
	}
	//Find matching VIC cell for each AMBHAS point
	count=0;
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			for (i=0; i< nvic; i++){

				if (fabs(d->lattitude[crow]-(l+i)->latitude)<gridsize/100.0 && fabs(d->longitude[ccol]- (l+i)->longitude)<gridsize/100.0){
         
					d->V_ncell[crow][ccol]=(l+i)->global_idx; //do this just for the global domain
					(l+i)->A_nlat=(size_t)crow;
					(l+i)->A_nlon=(size_t)ccol;                                   
					printf( "Link_AMBHAS_Local_VIC_Domain: ncell matching - %d - A_nlat %d, A_nlon %d VIC lat %f VIC lon %f A lat %f A lon %f \n", i, (int)(l+i)->A_nlat, (int)(l+i)->A_nlon, (l+i)->latitude, (l+i)->longitude, d->lattitude[crow],  d->longitude[ccol]);
					i=nvic-1;
				}
			}		 				
		}
	}

	if (debug==1){
		printf("\n");
		printf(" lons for each AMBHAS col:\n");
		for (ccol=0; ccol<d->NCOL; ccol ++){
			printf("%f " , d->longitude[ccol]);
		}	
		printf("\n");

		printf(" lats for each AMBHAS col:\n");
		for (crow=0; crow<d->NROW; crow ++){
			printf("%f " , d->lattitude[crow]);
		}	
		printf("\n");
		printf("V_ncell for each AMBHAS cell:\n");
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				printf(" %d ",(int)d->V_ncell[crow][ccol]);
				// printf("%f, %f, ", d->lattitude[crow], d->longitude[ccol]);
			}
			printf("\n");
		}
		printf("\n");

	}//end debug
	debug=0;
	if (debug==1){
		printf("Rank %d - Local Domain: A_nlat, A_nlons for each VIC cell:\n", mpi_rank);
		for (i=0; i< nvic; i++){
			printf( "ncell - %d - A_nlat %d, A_nlon %d VIC lat %f VIC lon %f A lat %f A lon %f \n", i, (int)(l+i)->A_nlat, (int)(l+i)->A_nlon, (l+i)->latitude, (l+i)->longitude, d->lattitude[(l+i)->A_nlat],  d->longitude[(l+i)->A_nlon]);
		}
	}
}
