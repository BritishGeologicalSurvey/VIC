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


int get_AMBHAS_Data_Into_Global_VIC(const gw_data_struct *d, const gw_param_struct *p, domain_struct *v, size_t time_step){
	location_struct *l;
 	l=v->locations;
	int crow,ccol,count, i;	
	count=0;
	//loop through all active cells of VIC
	printf("get_AMBHAS_Data_Into_Global_VIC: ncells total %d, ncells active %d \n", (int)v->ncells_total, (int)v->ncells_active);
   	for (i = 0; i < (int)v->ncells_total; i++) {		
	   	if(time_step==0){
			if(((l+i)->A_nlat>=0 )&& ((l+i)->A_nlon>=0)){ //only do it if A_nlat and A_nlon are valid cells (not minus!!)
				(l+count)->Sy=p->Sy[(l+count)->A_nlat][(l+count)->A_nlon];
				(l+count)->Kaq=d->K[(l+count)->A_nlat][(l+count)->A_nlon];
				(l+count)->z=(d->dem[(l+count)->A_nlat][(l+count)->A_nlon]-p->h[(l+count)->A_nlat][(l+count)->A_nlon]);

				//printf("get_AMBHAS_Data_Into_Global_VIC: A_nlat %d, A_nlon %d, i %d, count %d, Sy %f, z %f \n", (l+count)->A_nlat,(l+count)->A_nlon,i,count,(l+count)->Sy, (l+count)->z);
            }
            else{
            printf("!!!invalid cell for in get_AMBHAS_Data_Into_Global_VIC: A_nlat %d, A_nlon %d, i %d, count %d, Sy %f, z %f \n", (l+count)->A_nlat,(l+count)->A_nlon,i,count,(l+count)->Sy, (l+count)->z);
            
            }
			//printf("get_AMBHAS_Data_Into_Global_VIC: A_nlat %d, A_nlon %d, i %d, count %d, Sy %f, z %f \n", (l+count)->A_nlat,(l+count)->A_nlon,i,count,(l+count)->Sy, (l+count)->z);
			count ++;

		}
		if(time_step>0){
			if(((l+i)->A_nlat>=0 )&& ((l+i)->A_nlon>=0)){
				//update the depth to the water table for every time step
				(l+count)->z=d->dem[(l+count)->A_nlat][(l+count)->A_nlon]-p->h[(l+count)->A_nlat][(l+count)->A_nlon];
				(l+count)->Sy=p->Sy[(l+count)->A_nlat][(l+count)->A_nlon];
				//	printf("get_AMBHAS_Data_Into_Global_VIC: A_nlat %d, A_nlon %d, i %d, z %f Sy %f \n", (l+i)->A_nlat,(l+i)->A_nlon,count,(l+i)->z,  (l+i)->Sy);
			}
			else{
				printf("!!!invalid cell for in get_AMBHAS_Data_Into_Global_VIC: A_nlat %d, A_nlon %d, i %d, count %d, Sy %f, z %f \n", (l+count)->A_nlat,(l+count)->A_nlon,i,count,(l+count)->Sy, (l+count)->z);
			}    
        count ++;                                                                          
		}
	}
}