/******************************************************************************
* link_AMBHAS_VIC_Domain.c
* find nrow ncol for each VIC cell and find VIC ncell for each nrow/ncol 
* 
*****************************************************************************/

#include "Link_AMBHAS_VIC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int link_AMBHAS_VIC_Domain(gw_data_struct *d, domain_struct *v){
	int crow, ccol, count, i;
	count = 0;

	location_struct *l;


	//get lat lon from VIC
	l=v->locations;


	//Find matching VIC cell for each AMBHAS point
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			d->V_ncell[crow][ccol]=-999;//initialise to -999
			for (i=0; i< v->ncells_active; i++){
				(l+i)->A_nlat=-999;//initialise to -999
				(l+i)->A_nlon=-999;//initialise to -999
				if ((d->lattitude[crow] == (l+i)->latitude) && (d->longitude[ccol] == (l+i)->longitude)){
					d->V_ncell[crow][ccol]=i;
					(l+i)->A_nlat=(size_t)ccol;
					(l+i)->A_nlon=(size_t)crow;
					i=v->ncells_active-1;
				}

			}		 				
		}
	}
	printf("VIC_ncell for each AMBHAS cell:\n");
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			printf(" %.1d ",d->V_ncell[crow][ccol]);
		}
		printf("\n");
	}
	printf("\n");



	
return 0;
}
