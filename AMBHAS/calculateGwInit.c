/******************************************************************************
* calculateGWInit
* adjust transmissivity values at the boundary cells, 
* set h values at the boundary nodes and set h values outside the boundary to 
* missing values
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

int calculateGwInit(const gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p){ // run after the first time step of VIC

	int crow=0; int ccol=0; int count=0;//counters
	count=0;

	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			

			//calculate the area of each cell if it is a regular grid
			if(g->GRID==0){
				d->area[crow][ccol]=(double)(g->DLAT*g->DLON);
			}
			//if the aquifer is confined and K is read in, update transmissivity once
			if(g->KorTRANS==0 && g->CONFINED==1){
					
				p->Trans[crow][ccol]=d->K[crow][ccol]*(d->dem[crow][ccol]-d->zbase[crow][ccol]);
			}
			//if parts of the aquifer are confined, as specified by the aquiferMap, then set the transmissivity
			// for the confined cells once
			if(g->KorTRANS==0 && g->CONFINED==0){
				if(d->aquiferMap[crow][ccol]==0.0){
				p->Trans[crow][ccol]=d->K[crow][ccol]*(d->dem[crow][ccol]-d->zbase[crow][ccol]);
				}
			}

			//if the aquifer is confined and Transmissivity is read in, store it in the gw_param_struct
			if(g->KorTRANS==1){
				p->Trans[crow][ccol]=d->Trans[crow][ccol];

			}

			//set gw_inflow to 0
			d->gw_inflow[crow][ccol]=0.0;
			//set recharge to 0
			d->recharge[crow][ccol]=0.0;
			//set pumping to 0, will be overwritten if it is read in
			d->pumping[crow][ccol]=0.0;
      

		}
	}
	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
		/*set Trans of the cells just outside the boundary cell to the cell either
		 n, s, e, w of the boundary node to insure no-flow in the spatial modelling.*/
            		//if((mask[crow][ccol]==0.0)){
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
			//if it is a corner boundary node, it won't be used in the model

			}

			if(d->headBC[crow][ccol]>-999.0){
				//Set h to the value in headBC where it is a fixed head cell
				p->h[crow][ccol]=d->headBC[crow][ccol];
			}
			//Set h to -999 if its outside the mask and is not a headBC cell
			if((d->mask[crow][ccol]==0.0) && (d->headBC[crow][ccol]==-999.0)){
				p->h[crow][ccol]=-999.0;
			}
		}
 	}

} //end calculateGwInit
