/******************************************************************************
* readGlobalData
* Function that reads global parameters and observation bh points
*****************************************************************************/
#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>


int readGlobalData(gw_global_data_struct *g, gw_data_struct *d){
	char name[50];
	char hininame[100];
	char gwname[100];
	int count;
	printf("function readGlobalData is called: \n");
	FILE *fp;
	fp=fopen("..//AMBHAS//gw_observation.dat","r");//open file for reading
	if (fp==NULL){
		fprintf(stderr, "Can't open file in.list!\n");
		exit(1);
	}
	fscanf(fp,"%s %d",name, &(g->NUMOBSBH));

	printf("file is opened: \n");

	//allocate arrays of gw_global_data_struct
	g->p_obslat=(float*)malloc(g->NUMOBSBH*sizeof(float));
	g->p_obslon=(float*)malloc(g->NUMOBSBH*sizeof(float));
	g->p_obslat_count=(int*)malloc(g->NUMOBSBH*sizeof(int));
	g->p_obslon_count=(int*)malloc(g->NUMOBSBH*sizeof(int));
	printf("arrays are created: \n");

	for(count=0; count<g->NUMOBSBH;count++){
		fscanf(fp,"%s %f %f",name, &(g->p_obslat[count]), &(g->p_obslon[count]));
	}
	fclose(fp);

	printf("Observation Borehole No, lat, lon: \n");

	for (count=0; count<g->NUMOBSBH; count ++){
		printf(" %d                     %.1f   %.1f ",count, g->p_obslat[count],g->p_obslon[count]);
		printf("\n");
	}
	printf("\n");

	fp=fopen("..//AMBHAS//gw_global_parameters.dat","r");//open file for reading
	if (fp==NULL){
		fprintf(stderr, "Can't open file in.list!\n");
		exit(1);
	}
	fscanf(fp, " %s  %s",name, gwname);
	fscanf(fp," %s %d",name, (&g->NROW));
	fscanf(fp," %s %d",name, (&g->NCOL));
	fscanf(fp," %s %d",name, (&g->NTIME));
	fscanf(fp," %s %f",name, (&g->DLAT));
	fscanf(fp," %s %f",name, (&g->DLON));
	fscanf(fp," %s %f",name, (&g->DT));
	fscanf(fp," %s %d",name, (&g->OUT_OPTION));
	fscanf(fp, " %s %d",name, (&g->SIM_MODE));

	fscanf(fp, " %s %d %s",name, (&g->RESTART), hininame);
	fscanf(fp, " %s %d",name, (&g->KorTRANS));
	fscanf(fp, " %s %d",name, (&g->CONFINED));
	fscanf(fp, " %s %d",name, (&g->GRID));
	fscanf(fp, " %s %d",name, (&g->PUMPING));

 	fscanf(fp, " %s %d",name, (&g->VARIABLE_SY));
 	fscanf(fp, " %s %f",name, (&g->MAX_RECHARGE_FRACTION));

	fclose(fp);

	//write NROW and NCOL into gw_data_struct
	d->NROW=g->NROW;
	d->NCOL=g->NCOL;

	

	printf("NROW, NCOL, NTIME: \n");
	printf("%d    %d    %d \n",(g->NROW), (g->NCOL), (g->NTIME));
	printf("NROW, NCOL in data struct \n");
	printf("%d    %d     \n",(d->NROW), (d->NCOL));
	printf("DLAT, DLON, DT: \n");
	printf("%f    %f    %f \n",(g->DLAT), (g->DLON), (g->DT));
	printf("OUT_OPTION, SIM_MODE,  RESTART, KorTrans, CONFINED, GRID, VARIABLE_SY: \n");
	printf("%d    %d    %d    	 %d	%d	%d %d   \n",(g->OUT_OPTION), (g->SIM_MODE),  (g->RESTART), (g->KorTRANS), (g->CONFINED), (g->GRID), (g->VARIABLE_SY));
	printf("\n");
	printf("*** %s \n", hininame);

	strncpy(g->hininame, hininame, 100);
	printf("*** %s \n", g->hininame);
 
	strncpy(g->gwname, gwname, 100);
	printf("*** %s \n", g->gwname);

}
