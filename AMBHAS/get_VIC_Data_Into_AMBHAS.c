/******************************************************************************
* save recharge calculated in VIC into AMBHAS
* 
* 
*****************************************************************************/
#include "Link_AMBHAS_VIC.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


int get_VIC_Data_Into_AMBHAS(gw_data_struct *d, const gw_global_data_struct *g, domain_struct *v, double out_recharge[], double out_gw_inflow[]){
	//extern double           ***out_data;
	location_struct *l;
 	l=v->locations;
	int crow,ccol, i, count;
	int debug =0;
  
 	if (debug==1){
 	    printf("in get_VIC_Data_Into_AMBHAS ncells_active, %d \n ", v->ncells_active);
	}
	count=0;
	//loop through all active cells of VIC
   	for (i = 0; i <(int) v->ncells_total; i++) {//changed from ncells_active
		if((l+i)->run==0){ //check wether cell i is run
			if (debug==1){
				printf("cell %d is NOT run! \n",i);
			}
		}
		if((l+i)->run==1){ //check wether cell i is run
			if (debug==1){
				printf("cell %d is  run! \n",i);
					printf(" Important: A_nlat %d, A_nlon %d , g->DT %f - i %d - count %d \n", (l+i )->A_nlat, (l+i)->A_nlon, g->DT, i, count);
			}
			if(((l+i)->A_nlat>=0 )&& ((l+i)->A_nlon>=0)){ //only do it if A_nlat and A_nlon are valid cells (not minus!!)
				d->recharge[(l+i)->A_nlat][(l+i)->A_nlon]=out_recharge[i]/(1000.0*g->DT);//recharge in VIC is in mm/ts, in AMBHAS it is m/d //was out_recharge[count]
				d->gw_inflow[(l+i)->A_nlat][(l+i)->A_nlon]+=out_gw_inflow[i]/(1000.0*g->DT);//gw_inflow in VIC is in mm/ts, in AMBHAS it is m/d // was out_gw_inflow[count]
				if (debug==1){
					printf("---> Check (V2A): i %d, out_recharge %f, d_recharge %f, out_gw_inflow %f, d_gw_inflow %f\n", 
					i, out_recharge[i]/(1000.0*g->DT),  d->recharge[(l+i)->A_nlat][(l+i)->A_nlon], out_gw_inflow[i], 
					d->gw_inflow[(l+i)->A_nlat][(l+i)->A_nlon]);
				}
				count++;
			}
			else{
				printf("!!!invalid cell for in get_VIC_Data_Into_AMBHAS: A_nlat %d, A_nlon %d, i %d, count %d \n", (l+count)->A_nlat,(l+count)->A_nlon,i,count);
					
			}
		}
	}
	return 0;
}
