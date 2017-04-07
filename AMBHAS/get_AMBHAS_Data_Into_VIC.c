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
	location_struct *p_locations;
	int crow,ccol,count, i;	

	//loop through all active cells of VIC
   	for (i = 0; i < v->ncells_active; i++) {		
		//loop through all AMBHAS cells
		for(crow=0; crow<d->NROW; crow++){
			for(ccol=0; ccol<d->NROW; ccol++){
				//if V_ncell matches the count of the active cell, cet Kaq to K
				if(i==d->V_ncell[crow][ccol]){
					//only transfer Kaq for the first time step, it does not change over time
					if(time_step==0){
						(p_locations+i)->Kaq=d->K[crow][ccol];
					}
					//update the depth to the water table for every time step
					(p_locations+i)->z=d->dem[crow][ccol]-p->h[crow][ccol];
				}
			}
		}
	}
}
