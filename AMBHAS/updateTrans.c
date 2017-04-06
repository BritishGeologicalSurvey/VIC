/******************************************************************************
* updateTrans
*
* updates the Transmissivity for every time step for
* an unconfined aquifer. For an unconfined
* aquifer, the h value of the previous time step is used.
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
int updateTrans(const gw_data_struct * d, gw_param_struct *p){
	int crow, ccol, count;

	count =0;

	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			p->Trans[crow][ccol]=d->K[crow][ccol]*(p->h[crow][ccol]-d->zbase[crow][ccol]);
			//if it is a headBC value
			if(d->headBC[crow][ccol]>-999.0){
				p->Trans[crow][ccol]=d->K[crow][ccol]*(d->headBC[crow][ccol]-d->zbase[crow][ccol]);
			}
			/*set Trans of the cells just outside the boundary cell to the cell either
		 	n, s, e, w of the boundary node to insure no-flow in the spatial modelling.*/

            		if((d->mask[crow][ccol]==0.0)&&(d->headBC[crow][ccol]==-999.0)){

				if((crow>0)&&(d->mask[(crow-1)][ccol]==1.0)){
					p->Trans[crow][ccol]=p->Trans[(crow-1)][ccol];
				}
				if((crow<(d->NROW-1))&&(d->mask[(crow+1)][ccol]==1.0)){
					p->Trans[crow][ccol]=p->Trans[(crow+1)][ccol];
				}
				if((ccol>0)&&(d->mask[crow][(ccol-1)]==1.0)){
					p->Trans[crow][ccol]=p->Trans[crow][(ccol-1)];
				}
				if((ccol<(d->NCOL-1))&&(d->mask[crow][(ccol+1)]==1.0)){
					p->Trans[crow][ccol]=p->Trans[crow][(ccol+1)];
				}
			}
			count++;
		}	
	}
	//update the ones outside the mask and surrounding the head BC nodes
	count =0;
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			if(d->mask[crow][ccol]==0.0){
				if((crow>0)&&(d->headBC[(crow-1)][ccol]>-999.0)){
					p->Trans[crow][ccol]=p->Trans[(crow-1)][ccol];
				}
				if((crow<(d->NROW-1))&&(d->headBC[(crow+1)][ccol]>-999.0)){
					p->Trans[crow][ccol]=p->Trans[(crow+1)][ccol];
				}
				if((ccol>0)&&(d->headBC[crow][(ccol-1)]>-999.0)){
					p->Trans[crow][ccol]=p->Trans[crow][(ccol-1)];
				}
				if((ccol<(d->NCOL-1))&&(d->headBC[crow][(ccol+1)]>-999.0)){
					p->Trans[crow][ccol]=p->Trans[crow][(ccol+1)];
				}
				//if it is a corner boundary node, it won't be used in the model

            		}

		}
	}
	/*printf("Trans end of modification:\n");
	count=0;
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				printf(" %.f ", p->Trans[crow][ccol]);
				count++;
			}
			printf("\n");
		}
	printf("\n");*/
}
