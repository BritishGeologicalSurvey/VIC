/******************************************************************************
* save baseflow from AMBHAS into VIC
* 
* 
*****************************************************************************/
#include "Link_AMBHAS_VIC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int get_AMBHAS_Output_Into_VIC(const gw_data_struct *d, const gw_param_struct *p, domain_struct *v){

	int crow,ccol,count, i;	
	extern double           ***out_data;

	//loop through all active cells of VIC
   	for (i = 0; i < v->ncells_active; i++) {		
		//loop through all AMBHAS cells
		for(crow=0; crow<d->NROW; crow++){
			for(ccol=0; ccol<d->NROW; ccol++){
				//if V_ncell matches the count of the active cell, cet Kaq to K
				if(i==d->V_ncell[crow][ccol]){
					//set out_data in VIC to the baseflow from AMBHAS
					out_data[i][OUT_BASEFLOW][0]=p->baseflow[crow][ccol];
				}
			}
		}
	}
}
