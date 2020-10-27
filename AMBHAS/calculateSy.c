/******************************************************************************
* calculateSy
*
* calculates the specific yield of the water table depth for one time step.
* using a specific yield in the soil,one in the aquifer and a linear transition
* to twice the soil thickness. 
* 
* 
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
int calculateSy(const gw_data_struct * d, gw_param_struct *p){
	int crow, ccol;
 	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){
			// printf("%f ",d->z_soil[crow][ccol] );
			if((d->mask[crow][ccol]==1.0)){//only do cells within the mask
		  
			  //if dem-h > 2*z_soil, Sy=Sy of the aquifer
				if((d->dem[crow][ccol]-p->h[crow][ccol])>=(2*d->z_soil[crow][ccol])){
					p->Sy[crow][ccol]=d->Sy_aq[crow][ccol];     
				}
			  
			  
				//if dem-h<zsoil, Sy=Sy of the soil
				else if((d->dem[crow][ccol]-p->h[crow][ccol])<=(d->z_soil[crow][ccol])){
					p->Sy[crow][ccol]=d->Sy_soil[crow][ccol];    
				}    
			  
			  //if dem-h > 2*z_soil && h < z_soil, Sy=((Sy-Sy_soil)/z_soil)*(dem-h-zsoil)+Sy_soil
			  
				else{
					p->Sy[crow][ccol]=((d->Sy_aq[crow][ccol] -d->Sy_soil[crow][ccol])/d->z_soil[crow][ccol])
					*(d->dem[crow][ccol]-p->h[crow][ccol]-d->z_soil[crow][ccol])+d->Sy_soil[crow][ccol];

				}        
			}
			 //if outside the mask, set to Sy_aq
			else if((d->mask[crow][ccol]!=1.0)){
				p->Sy[crow][ccol]=d->Sy_aq[crow][ccol]; 

			}
		}
	}
 }