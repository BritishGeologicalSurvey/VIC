/******************************************************************************
* save recharge calculated in VIC into AMBHAS
* 
* 
*****************************************************************************/
#include "GW_global_vars.h"

//#include <vic_run.h>
//#include "vic_driver_shared_image.h"
//#include "vic_structs.h"//just for testing, vic_run.h is for running
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int get_VIC_Data_Into_AMBHAS(gw_data_struct *d, domain_struct *v){
	extern double           ***out_data;
	location_struct *l;
	int crow,ccol, i;
	
	//loop through all active cells of VIC
   	for (i = 0; i < v->ncells_active; i++) {
		
		//loop through all AMBHAS cells
		for(crow=0; crow<d->NROW; crow++){
			for(ccol=0; ccol<d->NROW; ccol++){

				//if V_ncell matches the count of the active cell, cet Kaq to K
				if(i==d->V_ncell[crow][ccol]){

					//set recharge in AMBHAS to out_data in VIC. I hope this is correct!!
					d->recharge[crow][ccol]=out_data[i][OUT_RECHARGE][0];

				}
				
			}
		}
	}
}
