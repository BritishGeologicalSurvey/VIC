/******************************************************************************
* GW_read_data
* This function reads in netcdf data that has been prepared in R, transforms it 
* and writes them into the gw_data_struct
*****************************************************************************/
#include "GW_global_vars.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}//error statement for netcdf data

int GW_read_data(const gw_global_data_struct *g, gw_data_struct *d){

	//printf("GW_read_data is called \n");

 	/* This is the name of the data file we will read. */
	char file_name[100]= "..//AMBHAS//";
	strcat(file_name, g->gwname);
	printf("*** %s \n", file_name);

	/* We are reading 2D data. */
	size_t ndims= 2;
	char lat_name[20]= "Lat";
	char lon_name[20]= "Lon";
	int NROW;
	int NCOL;
	NROW=g->NROW;
	NCOL=g->NCOL;


	/* Names of things. */
	char var1_name[20]= "Sy";
	char var2_name[20]= "T";
	char var3_name[20]= "K";
	char var4_name[20]= "mask";
	char var5_name[20]= "DEM";
	char var6_name[20]= "zbase";
	char var7_name[20]= "zriver";
	char var8_name[20]= "driver";
	char var9_name[20]= "C_eff";
	char var10_name[20]= "C_in";
	char var11_name[20]= "C_leak_eff";
	char var12_name[20]= "C_leak_in";
	char var13_name[20]= "headBC";
	char var14_name[20]="river_area";
	char var15_name[20]="aquifer_map";
	char var16_name[20]="dist_c_n";
	char var17_name[20]="dist_c_e";
	char var18_name[20]="edge_n";
	char var19_name[20]="edge_e";
	char var20_name[20]="cell_area";
 	char var21_name[20]="z_soil";
	char var22_name[20]="Sy_soil";


	char UNITS[20] ="units";

	/* For the units attributes. */
	char var1_units[20]= "-";
	char var2_units[20]= "m2/day";
	char var3_units[20]= "m/day";
	char var4_units[20]= "-";
	char var5_units[20]= "m";
	char var6_units[20]= "m";
	char var7_units[20]= "m";
	char var8_units[20]= "m";
	char var9_units[20]= "1/day";
	char var10_units[20]= "1/day";
	char var11_units[20]= "1/day";
	char var12_units[20]= "1/day";
	char var13_units[20]= "m";
	char var14_units[20]= "m2";
	char var15_units[20]= "-";
	char var16_units[20]= "m";
	char var17_units[20]= "m";
	char var18_units[20]= "m";
	char var19_units[20]= "m";
	char var20_units[20]= "m2";
 	char var21_units[20]= "m";
	char var22_units[20]= "-";

	char lat_units[20]= "degrees_north";
	char lon_units[20]= "degrees_east";

	int ncid, varid1, varid2, varid3, varid4, varid5, varid6, varid7, varid8, varid9, varid10, varid11, 
varid12, varid13, varid14, varid15, varid16, varid17, varid18, varid19, varid20, varid21, varid22;
	int lat_varid, lon_varid;
	int count;


	/* The start and counter arrays will tell the netCDF library where to
	read our data. */
	size_t start[ndims], counter[ndims];

	/* Program variables to hold the data we will read. We will only
	need enough space to hold one timestep of data; one record. */
	float var1_in[NROW][NCOL];
	float var2_in[NROW][NCOL];
	float var3_in[NROW][NCOL];
	float var4_in[NROW][NCOL];
	float var5_in[NROW][NCOL];
	float var6_in[NROW][NCOL];
	float var7_in[NROW][NCOL];
	float var8_in[NROW][NCOL];
	float var9_in[NROW][NCOL];
	float var10_in[NROW][NCOL];
	float var11_in[NROW][NCOL];
	float var12_in[NROW][NCOL];
	float var13_in[NROW][NCOL];
	float var14_in[NROW][NCOL];
	float var15_in[NROW][NCOL];
	float var16_in[NROW][NCOL];
	float var17_in[NROW][NCOL];
	float var18_in[NROW][NCOL];
	float var19_in[NROW][NCOL];
	float var20_in[NROW][NCOL];
 	float var21_in[NROW][NCOL];
 	float var22_in[NROW][NCOL];

	/* These program variables hold the latitudes and longitudes. */
	float lats[NROW], lons[NCOL];

	/* Loop indexes. */
	int lvl, lat, lon, rec, i = 0;

	/* Error handling. */
	int retval;

	/* Open the file. */
	if ((retval = nc_open(file_name, NC_NOWRITE, &ncid)))
		ERR(retval);

	printf("*** Flag1 \n");

	/* Get the varids of the latitude and longitude coordinate
	* variables. */
	if ((retval = nc_inq_varid(ncid, lat_name, &lat_varid)))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, lon_name, &lon_varid)))
		ERR(retval);

	printf("*** Flag2 \n");
	/* Read the coordinate variable data. */
	if ((retval = nc_get_var_float(ncid, lat_varid, &lats[0])))
		ERR(retval);
	if ((retval = nc_get_var_float(ncid, lon_varid, &lons[0])))
		ERR(retval);

	printf("*** Flag3 \n");

	/* Get the varids of  netCDF variables. */
	if ((retval = nc_inq_varid(ncid, var1_name, &varid1)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var2_name, &varid2)))
		ERR(retval);

	printf("*** Flag a \n");
	if ((retval = nc_inq_varid(ncid, var3_name, &varid3)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var4_name, &varid4)))
		ERR(retval);
	printf("*** Flag b \n");

	if ((retval = nc_inq_varid(ncid, var5_name, &varid5)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var6_name, &varid6)))
		ERR(retval);

	printf("*** Flag c \n");
	if ((retval = nc_inq_varid(ncid, var7_name, &varid7)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var8_name, &varid8)))
		ERR(retval);
	printf("*** Flag d \n");

	if ((retval = nc_inq_varid(ncid, var9_name, &varid9)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var10_name, &varid10)))
		ERR(retval);
	printf("*** Flag e \n");

	if ((retval = nc_inq_varid(ncid, var11_name, &varid11)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var12_name, &varid12)))
		ERR(retval);

	printf("*** Flag f \n");
	if ((retval = nc_inq_varid(ncid, var13_name, &varid13)))
		ERR(retval);
	printf("*** Flag g \n");


	if ((retval = nc_inq_varid(ncid, var14_name, &varid14)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var15_name, &varid15)))
		ERR(retval);

	//if the grid is irregular, read in the following parameters
	if(g->GRID==1){

		if ((retval = nc_inq_varid(ncid, var16_name, &varid16)))
			ERR(retval);

		if ((retval = nc_inq_varid(ncid, var17_name, &varid17)))
			ERR(retval);

		if ((retval = nc_inq_varid(ncid, var18_name, &varid18)))
			ERR(retval);

		if ((retval = nc_inq_varid(ncid, var19_name, &varid19)))
			ERR(retval);

		if ((retval = nc_inq_varid(ncid, var20_name, &varid20)))
			ERR(retval);
	}
	if(g->VARIABLE_SY==1){
		if ((retval = nc_inq_varid(ncid, var21_name, &varid21)))
			ERR(retval);

		if ((retval = nc_inq_varid(ncid, var22_name, &varid22)))
			ERR(retval);
	}

	printf("*** Flag4 \n");

	/* Read the data. Since we know the contents of the file we know
	* that the data arrays in this program are the correct size to
	* hold one timestep. */

	counter[0] = NROW; //orig
	counter[1] = NCOL;


	start[0] = 0;
	start[1] = 0;




	/* Read and check one record at a time. */
	//for (rec = 0; rec < NREC; rec++)
	//{
	if ((retval = nc_get_vara_float(ncid, varid1, start,
	counter, &var1_in[0][0])))
		ERR(retval);
	//printf("*** Flag5a \n");
	if ((retval = nc_get_vara_float(ncid, varid2, start,
	counter, &var2_in[0][0])))
		ERR(retval);
	printf("*** Flag5b \n");
	if ((retval = nc_get_vara_float(ncid, varid3, start,
	counter, &var3_in[0][0])))
		ERR(retval);
	//printf("*** Flag5c \n");
	if ((retval = nc_get_vara_float(ncid, varid4, start,
	counter, &var4_in[0][0])))
		ERR(retval);
	printf("*** Flag5d \n");
	if ((retval = nc_get_vara_float(ncid, varid5, start,
	counter, &var5_in[0][0])))
		ERR(retval);
	//printf("*** Flag5e \n");
	if ((retval = nc_get_vara_float(ncid, varid6, start,
	counter, &var6_in[0][0])))
		ERR(retval);
	//printf("*** Flag5f \n");
	if ((retval = nc_get_vara_float(ncid, varid7, start,
	counter, &var7_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid8, start,
	counter, &var8_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid9, start,
	counter, &var9_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid10, start,
	counter, &var10_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid11, start,
	counter, &var11_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid12, start,
	counter, &var12_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid13, start,
	counter, &var13_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid14, start,
	counter, &var14_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid15, start,
	counter, &var15_in[0][0])))
		ERR(retval);
	if(g->GRID==1){
		if ((retval = nc_get_vara_float(ncid, varid16, start,
		counter, &var16_in[0][0])))
			ERR(retval);
		if ((retval = nc_get_vara_float(ncid, varid17, start,
		counter, &var17_in[0][0])))
			ERR(retval);
		if ((retval = nc_get_vara_float(ncid, varid18, start,
		counter, &var18_in[0][0])))
			ERR(retval);
		if ((retval = nc_get_vara_float(ncid, varid19, start,
		counter, &var19_in[0][0])))
			ERR(retval);
		if ((retval = nc_get_vara_float(ncid, varid20, start,
		counter, &var20_in[0][0])))
			ERR(retval);
	}
	if(g->VARIABLE_SY==1){
		if ((retval = nc_get_vara_float(ncid, varid21, start,
		counter, &var21_in[0][0])))
			ERR(retval);
		if ((retval = nc_get_vara_float(ncid, varid22, start,
		counter, &var22_in[0][0])))
			ERR(retval);
	}
	//printf("*** Flag5g \n");

	i = 0;

	printf("Sy_soil \n");
		for (lat = 0; lat < NROW; lat++){
			for (lon = 0; lon < NCOL; lon++){

			//get data into the right format (lat, lon) by transforming it!

				d->Sy_aq[lat][lon]=*(var1_in[0]+i);
				d->Trans[lat][lon]=*(var2_in[0]+i);
				d->K[lat][lon]=*(var3_in[0]+i);
				d->mask[lat][lon]=*(var4_in[0]+i);
				d->dem[lat][lon]=*(var5_in[0]+i);
				d->zbase[lat][lon]=*(var6_in[0]+i);
				d->zriver[lat][lon]=*(var7_in[0]+i);
				d->driver[lat][lon]=*(var8_in[0]+i);
				d->C_eff[lat][lon]=*(var9_in[0]+i);
				d->C_in[lat][lon]=*(var10_in[0]+i);
				d->C_leak_eff[lat][lon]=*(var11_in[0]+i);
				d->C_leak_in[lat][lon]=*(var12_in[0]+i);
				d->headBC[lat][lon]=*(var13_in[0]+i);
				d->river_area[lat][lon]=*(var14_in[0]+i);
				d->aquiferMap[lat][lon]=*(var15_in[0]+i);
				if(g->GRID==1){
					d->c_n[lat][lon]=*(var16_in[0]+i);
					d->c_e[lat][lon]=*(var17_in[0]+i);
					d->e_n[lat][lon]=*(var18_in[0]+i);
					d->e_e[lat][lon]=*(var19_in[0]+i);
					d->area[lat][lon]=*(var20_in[0]+i);

				}
				if(g->VARIABLE_SY==1){
					// printf("VARIABLE_SY=1\n");
					d->z_soil[lat][lon]=*(var21_in[0]+i);
					d->Sy_soil[lat][lon]=*(var22_in[0]+i);
				}
				//printf("%f ",d->Sy_soil[lat][lon]);
				i++;
			}
			//printf("\n");
		}

		/*
		for (lon = 0; lon < NCOL; lon++){
		//d->longitude[lon]=(int)(1000*lons[lon])/1000.0;
			d->longitude[lon]=lons[lon];
			printf("%f ", d->longitude[lon]);
		}
		printf("\n");
		printf("\n");
		for (lat = 0; lat < NROW; lat++){
			//d->lattitude[lat]=(int)(100*lats[lat])/100.0;
			d->lattitude[lat]=lats[lat];
			printf("%f ", d->lattitude[lat]);
		}
		printf("\n");
		printf("\n");
		*/

		/* Close the file. */
		if ((retval = nc_close(ncid)))
		ERR(retval);

		printf("*** SUCCESS reading file gw_data.nc!\n");

		

return 0;

}//end readGWData
