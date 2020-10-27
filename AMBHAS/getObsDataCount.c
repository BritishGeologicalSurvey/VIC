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
			if(count == 1){
				printf("obslat %f, lattitude %f, DLAT %f count %d \n" , g->p_obslat[count], (float)d->lattitude[crow],g->DLAT, crow );
			}
			if(((g->p_obslat[count])>=(float)d->lattitude[crow]-0.5*g->DLAT)&& 
				((g->p_obslat[count])<(float)d->lattitude[crow]+0.5*g->DLAT)){


				g->p_obslat_count[count]=crow;
						//printf(" %d ", g->p_obslat_count[count]);
				printf("found obslat %d \n", count);
			}
		}

		for (ccol=0; ccol<g->NCOL; ccol++){
			if(count == 1){
				printf("obslon %f, longitude %f, DLAT %f count %d \n" , g->p_obslon[count], (float)d->longitude[ccol],g->DLON,ccol );
			}
			if(((g->p_obslon[count])>=(float)d->longitude[ccol]-0.5*g->DLON)&& 
				((g->p_obslon[count])<(float)d->longitude[ccol]+0.5*g->DLON)){

				g->p_obslon_count[count]=ccol;
				//	printf(" %d \n", g->p_obslon_count[count]);
				printf("found obslon %d \n", count);

			}
		}
		printf("obs borehole no %d, obslatcount %d obsloncount %d lat %f lon %f  \n", count, g->p_obslat_count[count],  g->p_obslon_count[count], g->p_obslat[count],  g->p_obslon[count]);
	}
   
	for (count=0; count<g->NUMOBSBH; count++){
		printf("borehole %d obslat_count %d obslon_count %d \n", count, *(g->p_obslat_count+count), *(g->p_obslon_count+count) );
	}
	return 0;
}

