/******************************************************************************
* save baseflow from AMBHAS into VIC
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


int get_AMBHAS_Output_Into_VIC(gw_data_struct *p_gw_data, domain_struct *vic_domain){
	location_struct *p_locations;
	int crow,ccol,count, i;	
	int V_ncell[NROW][NCOL];
	double baseflow[NROW][NCOL];
	extern double           ***out_data;


	count=0;
	for(crow=0; crow<NROW; crow++){
		for(ccol=0; ccol<NROW; ccol++){

			baseflow[crow][ccol]=(p_gw_data+count)->K;
			V_ncell[crow][ccol]=(int)(p_gw_data+count)->V_ncell;
			count++;
		}

	}

	//loop through all active cells of VIC
   	for (i = 0; i < vic_domain->ncells_active; i++) {		
		//loop through all AMBHAS cells
		for(crow=0; crow<NROW; crow++){
			for(ccol=0; ccol<NROW; ccol++){
				//if V_ncell matches the count of the active cell, cet Kaq to K
				if(i==V_ncell[crow][ccol]){
					//set out_data in VIC to the baseflow from AMBHAS
					out_data[i][OUT_BASEFLOW][0]=baseflow[crow][ccol];
				}
			}
		}
	}
}
