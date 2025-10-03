/******************************************************************************
* readGlobalData
* Function that reads global parameters and observation bh points
*****************************************************************************/
#include "Grid_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>


int readGridData(grid_data_struct *grid){
	char name[50];
	char VIC_name[100];
	char FORCE_name[100];
 	char FORCE_Data_name[100]; 
	int count;
	printf("function readGridData is called: \n");
	FILE *fp;


	fp=fopen("..//grid//grid_global_params.dat","r");//open file for reading
	if (fp==NULL){
		fprintf(stderr, "Can't open file grid_global_params.dat\n");
		exit(1);
	}
	fscanf(fp, " %s  %s",name, VIC_name);
 	fscanf(fp, " %s  %s",name, FORCE_name);
 	fscanf(fp, " %s  %s",name, FORCE_Data_name);
	fscanf(fp," %s %d",name, (&grid->NROW_VIC));
	fscanf(fp," %s %d",name, (&grid->NCOL_VIC));
	fscanf(fp," %s %f",name, (&grid->DLAT_VIC));
	fscanf(fp," %s %f",name, (&grid->DLON_VIC));
	fscanf(fp," %s %f",name, (&grid->x_origin_VIC));
	fscanf(fp," %s %f",name, (&grid->y_origin_VIC));
 
	fscanf(fp," %s %d",name, (&grid->NROW_FORCE));
	fscanf(fp," %s %d",name, (&grid->NCOL_FORCE));
	fscanf(fp," %s %f",name, (&grid->DLAT_FORCE));
	fscanf(fp," %s %f",name, (&grid->DLON_FORCE));
	fscanf(fp," %s %f",name, (&grid->x_origin_FORCE));
	fscanf(fp," %s %f",name, (&grid->y_origin_FORCE));
 
	fclose(fp);

	strncpy(grid->VIC_name, VIC_name, 100);
	printf("*** %s \n", grid->VIC_name);
 
	strncpy(grid->FORCE_name,FORCE_name, 100);
	printf("*** %s \n", grid->FORCE_name);
 
 	strncpy(grid-> FORCE_Data_name, FORCE_Data_name, 100);
	printf("*** %s \n", grid->FORCE_name);
 

 
 	printf("end readGridData \n");
 
}
