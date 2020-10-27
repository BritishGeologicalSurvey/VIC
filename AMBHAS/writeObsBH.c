/******************************************************************************
* writeObsBH
* Function that writes observation h into .txt file
* flag=1 open file for writing
* flag=2 append file and write one time step
* flag=3 append file and write entire ts
*****************************************************************************/
#include "GW_global_vars.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>

int writeObsBH(const gw_global_data_struct *g, const gw_ts_struct *ts, int start, int nts, int flag){
	char name[20];
	int count;
	int i;

	/*printf("Observation Borehole No, lat, lon: \n");

		for (count=0; count<g->NUMOBSBH; count ++){
			printf(" %d                     %.1f   %.1f ",count, lats[count],lons[count]);
			printf("\n");
		}
		printf("\n");
	*/

	if(flag==1){
		FILE *fp;
		fp=fopen("..//AMBHAS_OUT//gw_observation_ts.out","w");//open file for writing
		if (fp==NULL){
			fprintf(stderr, "Can't open file in.list!\n");
			exit(1);
		}
		fprintf(fp,"time_step/coords ");
		for(i=0; i<(g->NUMOBSBH);i++){
			fprintf(fp,"%.2f/%.2f ",g->p_obslat[i], g->p_obslon[i]);
		}
		fprintf(fp,"baseflow_tot leakage_tot balance_tot \n");

		for(count=start; count<(nts+start); count++){
			fprintf(fp,"%d ", count);
			for(i=0; i<(g->NUMOBSBH);i++){
				fprintf(fp, "%.18f ", ts->h_obs[i][count]);
			}
			fprintf(fp, "%.12f ", ts->total_baseflow[count]);
			fprintf(fp, "%.12f ", ts->total_leakage[count]);
			fprintf(fp, "%.12f ", ts->balance[count]);
			fprintf(fp,"\n");
		}
		fclose(fp);
	}
	if(flag==2){
		FILE *fp;
		fp=fopen("..//AMBHAS_OUT//gw_observation_ts.out","a");//append file for writing
		if (fp==NULL){
			fprintf(stderr, "Can't open file in.list!\n");
			exit(1);
		}
		for(count=0; count<1; count++){
			fprintf(fp,"%d ", nts);
			for(i=0; i<(g->NUMOBSBH);i++){
				 fprintf(fp, "%.18f ", ts->h_obs[i][count]);
			}
			fprintf(fp, "%.12f ", ts->total_baseflow[count]);
			fprintf(fp, "%.12f ", ts->total_leakage[count]);
			fprintf(fp, "%.12f ", ts->balance[count]);
			fprintf(fp,"\n");
		}
		fclose(fp);
	}

	if(flag==3){
		FILE *fp;
		fp=fopen("..//AMBHAS_OUT//gw_observation_ts.out","a");//append file for writing
		if (fp==NULL){
			fprintf(stderr, "Can't open file in.list!\n");
			exit(1);
		}
		for(count=0; count<nts; count++){
			fprintf(fp,"%d ", count+start);
			for(i=0; i<(g->NUMOBSBH);i++){
				fprintf(fp, "%.18f ", ts->h_obs[i][count]);
			}
			fprintf(fp, "%.12f ", ts->total_baseflow[count]);
			fprintf(fp, "%.12f ", ts->total_leakage[count]);
			fprintf(fp, "%.12f ", ts->balance[count]);
			fprintf(fp,"\n");
		}
		fclose(fp);
	}

	if(flag==4){
		FILE *fp;
		fp=fopen("..//AMBHAS_OUT//gw_observation_ts.out","a");//append file for writing
		if (fp==NULL){
			fprintf(stderr, "Can't open file in.list!\n");
			exit(1);
		}
		for(count=nts-1; count<nts; count++){
			fprintf(fp,"%d ", count+start);
			for(i=0; i<(g->NUMOBSBH);i++){
				fprintf(fp, "%.18f ", ts->h_obs[i][count]);
			}
			fprintf(fp, "%.12f ", ts->total_baseflow[count]);
			fprintf(fp, "%.12f ", ts->total_leakage[count]);
			fprintf(fp, "%.12f ", ts->balance[count]);
			fprintf(fp,"\n");
		}
		fclose(fp);

	}

}
