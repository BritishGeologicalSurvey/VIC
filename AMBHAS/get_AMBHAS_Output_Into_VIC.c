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


int get_AMBHAS_Output_Into_VIC(const gw_data_struct *d, const gw_param_struct *p, domain_struct *v, double *outp_baseflow , double *outp_z, double *outp_sy){

	int crow,ccol,count, i;	
	extern double           ***out_data;
 	location_struct *l;
 	l=v->locations;

	//printf("in get_AMBHAS_Output_Into_VIC: ncells active %d ncells total %d \n", v->ncells_active,v->ncells_total);

 	count=0;
	//loop through all a cells of VIC
   	for (i = 0; i < v->ncells_total; i++) { 
		if((l+i)->run==1){ //check wether cell i is run
			//outp_data=out_data[i];
			*(outp_baseflow+i)=p->baseflowTotal[(l+i)->A_nlat][(l+i)->A_nlon]/
			(d->area[(l+i)->A_nlat][(l+i)->A_nlon])*1000.0;

			//*(outp_z+i)=(l+i)->z*1000.0;//stores z in outp_z in mm js changed for testing

			*(outp_z+i)=(d->dem[(l+i)->A_nlat][(l+i)->A_nlon]-
			p->h[(l+i)->A_nlat][(l+i)->A_nlon])*1000.0;


			*(outp_sy+i)=(p->Sy[(l+i)->A_nlat][(l+i)->A_nlon]);


			//printf("cell global %d  local %d baseflow %f z %f Sy %f \n", i, count, *(outp_baseflow +i), *(outp_z +i),*(outp_sy +i));

			count++;	

		}
	}
	return 0;
}
