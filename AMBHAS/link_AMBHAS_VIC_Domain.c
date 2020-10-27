/******************************************************************************
* link_AMBHAS_VIC_Domain.c
* find AMBHAS nrow ncol for each VIC cell and find VIC ncell for each AMBHAS nrow/ncol 
* for global domain
*****************************************************************************/

#include "Link_AMBHAS_VIC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void link_AMBHAS_VIC_Domain(gw_data_struct *d, domain_struct *v){
	int crow, ccol, count, i, nvic, nvic_t; 
	double gridsize;
	int debug =1;
	count = 0;
	printf("link AMBHAS VIC Domain is called:\n");
	location_struct *l;

	//get lat lon from VIC
	l=v->locations;
	nvic_t=(int) v->ncells_total;//was ncells_total but did not work, try ncells_active
	nvic=(int) v->ncells_active;//was ncells_total but did not work, try ncells_active

	if (debug==1){
		printf("Link AMBAS_VIC_Domain ncells_total in domain vic domain %d \n", nvic);
		printf("ncells_active in global vic domain %d \n", (int) v->ncells_active);
		printf("NROW %d \n", d->NROW);
		printf("NCOL %d \n", d->NCOL);
	}

	//initialise variables
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			d->V_ncell[crow][ccol]=-999;//initialise to -999
			//printf("AMBHAS_LAT %f, AMBHAS_LON %f \n",d->lattitude[crow], d->longitude[ccol]);
				 				
		}
	}

	for (i=0; i< nvic_t; i++){
		(l+i)->A_nlat=-999;//initialise to 999 has to be positive, as size_t
		(l+i)->A_nlon=-999;//initialise to 999
		//printf("VIC_LAT %f, VIC_LON %f global_idx %d local_idx %d \n", (l+i)->latitude, (l+i)->longitude, (l+i)->global_idx,  (l+i)->local_idx);
	}	

	//Find matching VIC cell for each AMBHAS point	
	gridsize=fabs(d->lattitude[1]-d->lattitude[0]);
	if (debug==1){
		printf("gridsize %f \n",gridsize );
		printf(" lattitude[0] %f, lattitude[1] %f, lattitude[2] %f\n", d->lattitude[0], d->lattitude[1], d->lattitude[2]);
	}

	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			for (i=0; i< nvic_t; i++){ //for each vic cell
				if (fabs(d->lattitude[crow] - (l+i)->latitude)<gridsize/10.0 && fabs(d->longitude[ccol] - (l+i)->longitude)<gridsize/10.0){
					d->V_ncell[crow][ccol]=(l+i)->global_idx;
					(l+i)->A_nlon=(size_t)ccol;
					(l+i)->A_nlat=(size_t)crow;
					printf("link_AMBHAS_VIC_Domain: A_lat %f, V_lat %f, A_lon %f, V_lon %f, V_cell %d, A_nlon %d, A_nlat %d \n",d->lattitude[crow] , (l+i)->latitude, d->longitude[ccol], (l+i)->longitude, i, ccol, crow );
				}
			}
		}		 				
	}
}
