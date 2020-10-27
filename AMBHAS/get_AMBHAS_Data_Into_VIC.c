/******************************************************************************
* save hydraulic conductivity from AMBHAS into VIC structure
* save water table depth calculated in AMBHAS for every time step into VIC
* 
*****************************************************************************/
#include "Link_AMBHAS_VIC.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int get_AMBHAS_Data_Into_VIC(const gw_data_struct *d, const gw_param_struct *p, domain_struct *v, size_t time_step){
	location_struct *l;
 	l=v->locations;
	int crow,ccol,count, i, ntasks;	
	extern double           ***out_data;
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

	//loop through all active cells of VIC
	printf("ncells_active %d \n", (int)(v->ncells_active) );
   	for (i = 0; i < (int)v->ncells_active; i++) {		
   		if(time_step==0){
    	//printf("get_AMBHAS_Data_Into_VIC: A_nlat %d, A_nlon %d, i %d, ncells_active %d, global_idx %d, local_idx %d, run %d \n", (l+i)->A_nlat,(l+i)->A_nlon,i, v->ncells_active,(l+i)->global_idx,(l+i)->local_idx, (l+i)->run);
		//printf("get_AMBHAS_Data_Into_VIC: A_nlat %d, A_nlon %d, i %d, ncells_active %d,  local_idx %d, run %d \n", (l+i)->A_nlat,(l+i)->A_nlon,i, v->ncells_active,(l+i)->local_idx, (l+i)->run);
     
			if(((l+i)->A_nlat>=0 )&& ((l+i)->A_nlon>=0)){ //only do it if A_nlat and A_nlon are valid cells (not minus!!)
				(l+i)->Sy=p->Sy[(l+i)->A_nlat][(l+i)->A_nlon];
				(l+i)->Kaq=d->K[(l+i)->A_nlat][(l+i)->A_nlon];
				(l+i)->z=(d->dem[(l+i)->A_nlat][(l+i)->A_nlon]-p->h[(l+i)->A_nlat][(l+i)->A_nlon]);

			}
			else{
				printf("!!!invalid cell in get_AMBHAS_Data_Into_VIC: for A_nlat %d, A_nlon %d, i %d, ncells_active %d,  local_idx %d, run %d \n", (l+i)->A_nlat,(l+i)->A_nlon,i, v->ncells_active,(l+i)->local_idx, (l+i)->run);
			}


		//printf("get_AMBHAS_Data_Into_VIC: A_nlat %d, A_nlon %d, i %d, ncells_active %d, global_idx %d, local_idx %d, Sy %f z %f \n", (l+i)->A_nlat,(l+i)->A_nlon,i, v->ncells_active,(l+i)->global_idx,(l+i)->local_idx, (l+i)->Sy, (l+i)->z);
		}
		//update the depth to the water table for every time step
		else{
			if(((l+i)->A_nlat>=0 )&& ((l+i)->A_nlon>=0)){ //only do it if A_nlat and A_nlon are valid cells (not minus!!)
						 (l+i)->z =out_data[i][OUT_Z][0]/1000.0; //convert back to m
						(l+i)->Sy =out_data[i][OUT_SY][0];
		//	printf("get_AMBHAS_Data_Into_VIC: A_nlat %d, A_nlon %d, i %d, z  %f \n", (l+i)->A_nlat,(l+i)->A_nlon,i,(l+i)->z);
			}
		}
	}
}
