/******************************************************************************
* getObsDataCount
* function to find the counters of the observation BH coordinates and assign it 
* to the global pointers p_obslat and p_obslon
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

int getObsDataCount(gw_global_data_struct *g, const gw_data_struct *d){
	int ccol, crow, count;

	//find count of observation borehole


		printf("obslat_count   obslon_count \n");
		for (count=0; count<g->NUMOBSBH; count++){
			for (crow=0; crow<g->NROW; crow++){
				if(((g->p_obslat[count])>=(float)d->lattitude[crow]-0.5*g->DLAT)&& 
					((g->p_obslat[count])<(float)d->lattitude[crow]+0.5*g->DLAT)){

					g->p_obslat_count[count]=crow;
					printf(" %d ", g->p_obslat_count[count]);
				}
			}

			for (ccol=0; ccol<g->NCOL; ccol++){
				if(((g->p_obslon[count])>=(float)d->longitude[ccol]-0.5*g->DLON)&& 
					((g->p_obslon[count])<(float)d->longitude[ccol]+0.5*g->DLON)){

					g->p_obslon_count[count]=ccol;
					printf(" %d \n", g->p_obslon_count[count]);

				}
			}
		}


	

}

