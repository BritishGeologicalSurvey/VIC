/******************************************************************************
* calculateGWFlow
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

float calculateGwFlow(const gw_global_data_struct *g, const gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, int n_time_step){

	int crow; int ccol; int count; int ctime; int i; int itercount; //counters
	double stability=0.0;
	double mints=1000000;;//min time step required for stability
	double numiter;
	double dtAquiferStorage=0.0;
	double sumBaseflow=0;
	double sumRecharge=0;
	double sumPumping=0;
	double totalBalance=0;
	count=0;
	i=0;

	
	//if K is read in and it is an unconfined aquifer, update Transmissivity once per time step 
	//using the head of the previous time step
	if((g->KorTRANS==0) && (g->CONFINED==0)){
		updateTrans(d, p);
	
	}

	/*for (crow=0; crow<NROW; crow++){
		for (ccol=0; ccol<NCOL; ccol ++){
			printf("%.f ",p->Trans[crow][ccol]);

		}
		printf("\n");
	}
	printf("\n");*/

	for (crow=0; crow<d->NROW; crow++){
		for (ccol=0; ccol<d->NCOL; ccol ++){

            		p->hini[crow][ccol]=-999.0;//initialise hini to -999
			p->baseflowTotal[crow][ccol]=0;//initialise baseflowTotal to 0

                	if(d->mask[crow][ccol]==1){
                    		
				//check for stability and find minimum number of iterations (mints)

				stability=d->area[crow][ccol]*d->Sy[crow][ccol]/(4*p->Trans[crow][ccol]);


				if(mints>stability){
					mints=stability;
				}
				//check for stability at the river nodes
				if(d->river_area[crow][ccol]>0.0){
					if(d->C_eff[crow][ccol]!=0.0){
						stability=d->river_area[crow][ccol]*d->Sy[crow][ccol]/(4*d->C_eff[crow][ccol]);
					}
					if(mints>stability){
						mints=stability;
					}
					if(d->C_in[crow][ccol]!=0.0){
						stability=d->river_area[crow][ccol]*d->Sy[crow][ccol]/(4*d->C_in[crow][ccol]);
					}
					if(mints>stability){
						mints=stability;
					}
				}
                    		
                	}//end if mask==1

		}
	}




	mints=1.0/mints;
	numiter=ceil(mints);

	if (numiter<1.0){
		numiter=1.0;
	}




	//iterate through time
	for(itercount=0; itercount<(int)numiter;itercount++){


		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				count=ccol+crow*d->NCOL;

				//if within the mask or headBC, initialize hini to h of the previous time step
				if((d->mask[crow][ccol]==1.0)|| (d->headBC[crow][ccol]!=-999.0)){
					p->hini[crow][ccol]=p->h[crow][ccol];
				}

				//if outside the mask, set h and baseflow to -999
		    		if(d->mask[crow][ccol]==0.0){
		        		p->h[crow][ccol]=-999.0;
		        		p->baseflow[crow][ccol]=-999.0;
		    		}

				//if it is a fixed head node, set h to hini, and baseflow to -999
			    	if(d->headBC[crow][ccol]!=-999.0){ //if it is a fixed head
					p->h[crow][ccol]=p->hini[crow][ccol];
					p->baseflow[crow][ccol]=-999.0;
			    	}
				//if it is in the mask and not a fixed head node:
			     	if((d->mask[crow][ccol]==1.0) && (d->headBC[crow][ccol]==-999.0)){

					//add recharge and pumping for one iteration and update h
					p->h[crow][ccol]=p->hini[crow][ccol]+g->DT/numiter*(d->recharge[crow][ccol]-
						d->pumping[crow][ccol]/d->area[crow][ccol])/d->Sy[crow][ccol];

					sumRecharge+=g->DT/numiter*d->recharge[crow][ccol]*d->area[crow][ccol];//m3/iteration
					sumPumping+=g->DT/numiter*d->pumping[crow][ccol]; //m3/iteration

			       		 /* calculate baseflow and leakage for one iteration as long as it
					 is not a fixed head node*/
					if(p->h[crow][ccol]<d->dem[crow][ccol]){
				    		p->baseflow[crow][ccol]=d->Sy[crow][ccol]*d->C_eff[crow][ccol]*
							d->river_area[crow][ccol]*(p->h[crow][ccol]-d->dem[crow][ccol])
							+d->Sy[crow][ccol]*d->C_leak_eff[crow][ccol]*d->area[crow][ccol]*
							(p->h[crow][ccol]-d->dem[crow][ccol]);//m3/day river to aquifer
					}
					else if(p->h[crow][ccol]>d->dem[crow][ccol]) {
				    		p->baseflow[crow][ccol]=d->Sy[crow][ccol]*d->C_in[crow][ccol]*
							d->river_area[crow][ccol]*(p->h[crow][ccol]-d->dem[crow][ccol])+
							d->Sy[crow][ccol]*d->C_leak_in[crow][ccol]*d->area[crow][ccol]*
							(p->h[crow][ccol]-d->dem[crow][ccol]); //aquifer to river
					}

					if(p->baseflow[crow][ccol]!=0.0){
					//add baseflow for one iteration and update h
						p->h[crow][ccol]=p->h[crow][ccol]-g->DT/numiter*p->baseflow[crow][ccol]/
							(d->Sy[crow][ccol]*d->area[crow][ccol]);

						//sum up total baseflow for each iteration 
						sumBaseflow+=p->baseflow[crow][ccol]*g->DT/numiter;

						//sum up baseflow for each iteration and cell
						p->baseflowTotal[crow][ccol]+=p->baseflow[crow][ccol]*g->DT/numiter;
					}
				}
			}
		}

		//get h for the n, s, e, w cell for spatial gw modelling
		for (ccol=0; ccol<d->NCOL; ccol ++){
			for (crow=0; crow<d->NROW; crow++){
				if(crow==0){
					p->hs[crow][ccol]=p->h[crow][ccol];
				}
				else if (crow>0){
					p->hs[crow][ccol]=p->h[crow-1][ccol];
				}
				if(crow==d->NROW-1){
					p->hn[crow][ccol]=p->h[crow][ccol];
				}
				else if (crow<d->NROW-1){
					p->hn[crow][ccol]=p->h[crow+1][ccol];
				}
			}
		}

		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				if(ccol==0){
					p->he[crow][0]=p->h[crow][0];
				}
				else if (ccol>0){
					p->he[crow][ccol]=p->h[crow][ccol-1];
				}
				if(ccol==d->NCOL-1){
					p->hw[crow][ccol]=p->h[crow][ccol];
				}
				else if (ccol<d->NCOL-1){
					p->hw[crow][ccol]=p->h[crow][ccol+1];
				}
			}
		}


		//where hn,hs,he,hw are -999, set them to h (as there might be a value)
		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){
				if(p->hn[crow][ccol]==-999.0){
		        		p->hn[crow][ccol]=p->h[crow][ccol];
				}
				if(p->hs[crow][ccol]==-999.0){
					p->hs[crow][ccol]=p->h[crow][ccol];
				}
				if(p->he[crow][ccol]==-999.0){
					p->he[crow][ccol]=p->h[crow][ccol];
				}
				if(p->hw[crow][ccol]==-999.0){
					p->hw[crow][ccol]=p->h[crow][ccol];
				}

			}
		 }


		for (crow=0; crow<d->NROW; crow++){
			for (ccol=0; ccol<d->NCOL; ccol ++){

				/*if it is not a fixed head and within the mask
				calculate the flux to the four adjacent cells.
				if the cell is at a no-flow boundary, the flux will be 0, 
				as the transmissivity of the adjacent
 				cell has been set to value of the central cell. */
	
		        	if((d->mask[crow][ccol]==1.0)&&(d->headBC[crow][ccol]==-999.0)){
					if(g->GRID==0){
						p->qn[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow+1][ccol])*
							(p->hn[crow][ccol]-p->h[crow][ccol])*g->DLON/g->DLAT; //m3/day
						p->qs[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol])*
							(p->hs[crow][ccol]-p->h[crow][ccol])*g->DLON/g->DLAT;
						p->qe[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow][ccol-1])*
							(p->he[crow][ccol]-p->h[crow][ccol])*g->DLAT/g->DLON;
						p->qw[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol+1])*
							(p->hw[crow][ccol]-p->h[crow][ccol])*g->DLAT/g->DLON;
					}
					if(g->GRID==1){
						//note: c_e=c_w and e_e=e_w
						p->qn[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow+1][ccol])*
							(p->hn[crow][ccol]-p->h[crow][ccol])*
							d->e_n[crow][ccol]/d->c_n[crow][ccol]; //m3/day
						p->qs[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol])*
							(p->hs[crow][ccol]-p->h[crow][ccol])*
							d->e_n[crow-1][ccol]/d->c_n[crow-1][ccol];
						p->qe[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow][ccol-1])*
							(p->he[crow][ccol]-p->h[crow][ccol])*
							d->e_e[crow][ccol]/d->c_e[crow][ccol];
						p->qw[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol+1])*
							(p->hw[crow][ccol]-p->h[crow][ccol])*
							d->e_e[crow][ccol]/d->c_e[crow][ccol];
					}



		        		p->error[crow][ccol]=((d->recharge[crow][ccol]*d->area[crow][ccol])
					-d->pumping[crow][ccol]-p->baseflow[crow][ccol]
					+(p->qw[crow][ccol]+p->qe[crow][ccol]+
					p->qs[crow][ccol]+p->qn[crow][ccol]))*g->DT/numiter+d->Sy[crow][ccol]*d->area[crow][ccol]/
					(p->hini[crow][ccol]-p->h[crow][ccol]);

		        		totalBalance+=p->error[crow][ccol];
		   		}
		   		 /*for the head boundary nodes, if there is a value for hn,s,e,w, 
				then calculate the flux, else don't. This will result in a 0 water balance. */

		    		if (d->headBC[crow][ccol]!=-999.0){
					if(g->GRID==0){

						if(p->hn[crow][ccol]!=-999.0){
				    			p->qn[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow+1][ccol])*
								(p->hn[crow][ccol]-p->h[crow][ccol])*g->DLON/g->DLAT;
						}
						if(p->hs[crow][ccol]!=-999.0){
				    			p->qs[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol])*
								(p->hs[crow][ccol]-p->h[crow][ccol])*g->DLON/g->DLAT;
						}
						if(p->he[crow][ccol]!=-999.0){
				    			p->qe[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow][ccol-1])*
								(p->he[crow][ccol]-p->h[crow][ccol])*g->DLAT/g->DLON;
						}
						if(p->hw[crow][ccol]!=-999.0){
				   			 p->qw[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol+1])*
								(p->hw[crow][ccol]-p->h[crow][ccol])*g->DLAT/g->DLON;
						}
					}
					if(g->GRID==1){

						if(p->hn[crow][ccol]!=-999.0){
				    			p->qn[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow+1][ccol])*
								(p->hn[crow][ccol]-p->h[crow][ccol])*
								d->e_n[crow][ccol]/d->c_n[crow][ccol];
						}
						if(p->hs[crow][ccol]!=-999.0){
				    			p->qs[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol])*
								(p->hs[crow][ccol]-p->h[crow][ccol])*
								d->e_n[crow-1][ccol]/d->c_n[crow-1][ccol];
						}
						if(p->he[crow][ccol]!=-999.0){
				    			p->qe[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow][ccol-1])*
								(p->he[crow][ccol]-p->h[crow][ccol])*
								d->e_e[crow][ccol]/d->c_e[crow][ccol];
						}
						if(p->hw[crow][ccol]!=-999.0){
				   			 p->qw[crow][ccol]=2/(1/p->Trans[crow][ccol]+1/p->Trans[crow-1][ccol+1])*
								(p->hw[crow][ccol]-p->h[crow][ccol])*
								d->e_e[crow][ccol]/d->c_e[crow][ccol];
						}
					}

		        		p->error[crow][ccol]=
		            		(p->qw[crow][ccol]+p->qe[crow][ccol]+p->qs[crow][ccol]+p->qn[crow][ccol])*g->DT/numiter;

		        		totalBalance+=p->error[crow][ccol];

		    		}

			}
		}

			for (crow=0; crow<d->NROW; crow++){
				for (ccol=0; ccol<d->NCOL; ccol ++){
				//if in the mask and not a fixed head BC
					if(d->mask[crow][ccol]==1.0 && d->headBC[crow][ccol]==-999.0){
						p->h[crow][ccol]=p->h[crow][ccol]+(g->DT/numiter)/(d->Sy[crow][ccol]*
							d->area[crow][ccol])*(p->qn[crow][ccol]+p->qs[crow][ccol]+
							p->qe[crow][ccol]+p->qw[crow][ccol]);

		        			dtAquiferStorage+=fabs((d->Sy[crow][ccol]*d->area[crow][ccol]/(g->DT/numiter)*
							(p->h[crow][ccol]-p->hini[crow][ccol])));
		    			}

				}
			}

	}//end iteration

  	//Do water balance of time step
/*
	  printf("cumulative waterbalance at %2d:\n",n_time_step);
	  printf("recharge = %.2f, pumping =  %.2f, baseflow =  %.2f,  m3/day, total= %.2f  m3/day 		\n",sumRecharge,sumPumping,sumBaseflow, sumRecharge-sumPumping-sumBaseflow);
	  printf("aquifer storage change = %.2f m3/day\n"  ,dtAquiferStorage);
	  printf("sum water balance of each cell= %.3f m3/day\n"  ,totalBalance);
*/
//write observation points
for (count=0; count<g->NUMOBSBH; count++){
	i=g->NUMOBSBH*n_time_step+count;
	ts->h_obs[count][n_time_step]=p->h[*(g->p_obslat_count+count)][*(g->p_obslon_count+count)];
	ts->balance[n_time_step]=dtAquiferStorage;
	ts->total_baseflow[n_time_step]=sumBaseflow;

}




return (float)dtAquiferStorage*g->DT;

}//end calculateGwFlow
