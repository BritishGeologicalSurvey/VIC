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


	/* This is the name of the data file we will read. */
	char file_name[20]= "gw_data.nc";

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
	char var7_name[20]= "C_eff";
	char var8_name[20]= "C_in";
	char var9_name[20]= "C_leak_eff";
	char var10_name[20]= "C_leak_in";
	char var11_name[20]= "headBC";
	char var12_name[20]="river_area";
	char var13_name[20]="dist_c_n";
	char var14_name[20]="dist_c_e";
	char var15_name[20]="edge_n";
	char var16_name[20]="edge_e";
	char var17_name[20]="cell_area";


	char UNITS[20] ="units";

	/* For the units attributes. */
	char var1_units[20]= "-";
	char var2_units[20]= "m2/day";
	char var3_units[20]= "m/day";
	char var4_units[20]= "-";
	char var5_units[20]= "m";
	char var6_units[20]= "m";
	char var7_units[20]= "1/day";
	char var8_units[20]= "1/day";
	char var9_units[20]= "1/day";
	char var10_units[20]= "1/day";
	char var11_units[20]= "m";
	char var12_units[20]= "m2";
	char var13_units[20]= "m";
	char var14_units[20]= "m";
	char var15_units[20]= "m";
	char var16_units[20]= "m";
	char var17_units[20]= "m2";

	char lat_units[20]= "degrees_north";
	char lon_units[20]= "degrees_east";

	int ncid, varid1, varid2, varid3, varid4, varid5, varid6, varid7, varid8, varid9, varid10, varid11, 
varid12, varid13, varid14, varid15, varid16, varid17;
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

	/* These program variables hold the latitudes and longitudes. */
	float lats[NROW], lons[NCOL];

	/* Loop indexes. */
	int lvl, lat, lon, rec, i = 0;

	/* Error handling. */
	int retval;

	/* Open the file. */
	if ((retval = nc_open(file_name, NC_NOWRITE, &ncid)))
		ERR(retval);

	//printf("*** Flag1 \n");

	/* Get the varids of the latitude and longitude coordinate
	* variables. */
	if ((retval = nc_inq_varid(ncid, lat_name, &lat_varid)))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, lon_name, &lon_varid)))
		ERR(retval);

	//printf("*** Flag2 \n");
	/* Read the coordinate variable data. */
	if ((retval = nc_get_var_float(ncid, lat_varid, &lats[0])))
		ERR(retval);
	if ((retval = nc_get_var_float(ncid, lon_varid, &lons[0])))
		ERR(retval);

	//printf("*** Flag3 \n");

	/* Get the varids of  netCDF variables. */
	if ((retval = nc_inq_varid(ncid, var1_name, &varid1)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var2_name, &varid2)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var3_name, &varid3)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var4_name, &varid4)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var5_name, &varid5)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var6_name, &varid6)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var7_name, &varid7)))
		ERR(retval);
	if ((retval = nc_inq_varid(ncid, var8_name, &varid8)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var9_name, &varid9)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var10_name, &varid10)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var11_name, &varid11)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var12_name, &varid12)))
		ERR(retval);
//if the grid is irregular, read in the following parameters
if(g->GRID==1){

	if ((retval = nc_inq_varid(ncid, var13_name, &varid13)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var14_name, &varid14)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var15_name, &varid15)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var16_name, &varid16)))
		ERR(retval);

	if ((retval = nc_inq_varid(ncid, var17_name, &varid17)))
		ERR(retval);
}

	//printf("*** Flag4 \n");

	/* Read the data. Since we know the contents of the file we know
	* that the data arrays in this program are the correct size to
	* hold one timestep. */

	//counter[0] = NROW; //orig
	//counter[1] = NCOL;
	counter[0] = NCOL;//needs to be this way to transform the data
	counter[1] = NROW;

	start[0] = 0;
	start[1] = 0;




	/* Read and check one record at a time. */
	//for (rec = 0; rec < NREC; rec++)
	//{
	if ((retval = nc_get_vara_float(ncid, varid1, start,
	counter, &var1_in[0][0])))
		ERR(retval);
//printf("*** Flag5 \n");
	if ((retval = nc_get_vara_float(ncid, varid2, start,
	counter, &var2_in[0][0])))
		ERR(retval);
//printf("*** Flag5 \n");
	if ((retval = nc_get_vara_float(ncid, varid3, start,
	counter, &var3_in[0][0])))
		ERR(retval);
//printf("*** Flag5 \n");
	if ((retval = nc_get_vara_float(ncid, varid4, start,
	counter, &var4_in[0][0])))
		ERR(retval);
//printf("*** Flag5 \n");
	if ((retval = nc_get_vara_float(ncid, varid5, start,
	counter, &var5_in[0][0])))
		ERR(retval);
//printf("*** Flag5 \n");
	if ((retval = nc_get_vara_float(ncid, varid6, start,
	counter, &var6_in[0][0])))
		ERR(retval);
//printf("*** Flag5 \n");
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

if(g->GRID==1){
	if ((retval = nc_get_vara_float(ncid, varid13, start,
	counter, &var13_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid14, start,
	counter, &var14_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid15, start,
	counter, &var15_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid16, start,
	counter, &var16_in[0][0])))
		ERR(retval);
	if ((retval = nc_get_vara_float(ncid, varid17, start,
	counter, &var17_in[0][0])))
		ERR(retval);
}
//printf("*** Flag5 \n");

	i = 0;


	for (lon = 0; lon < NCOL; lon++){
		for (lat = 0; lat < NROW; lat++){

		//get data into the right format (lat, lon) by transforming it!

			d->Sy[lat][lon]=*(var1_in[0]+i);
			d->Trans[lat][lon]=*(var2_in[0]+i);
			d->K[lat][lon]=*(var3_in[0]+i);
			d->mask[lat][lon]=*(var4_in[0]+i);
			d->dem[lat][lon]=*(var5_in[0]+i);
			d->zbase[lat][lon]=*(var6_in[0]+i);
			d->C_eff[lat][lon]=*(var7_in[0]+i);
			d->C_in[lat][lon]=*(var8_in[0]+i);
			d->C_leak_eff[lat][lon]=*(var9_in[0]+i);
			d->C_leak_in[lat][lon]=*(var10_in[0]+i);
			d->headBC[lat][lon]=*(var11_in[0]+i);
			d->river_area[lat][lon]=*(var12_in[0]+i);
			if(g->GRID==1){
				d->c_n[lat][lon]=*(var13_in[0]+i);
				d->c_e[lat][lon]=*(var14_in[0]+i);
				d->e_n[lat][lon]=*(var15_in[0]+i);
				d->e_e[lat][lon]=*(var16_in[0]+i);
				d->area[lat][lon]=*(var17_in[0]+i);

			}

			i++;
		}
	}

	for (lon = 0; lon < NCOL; lon++){
		d->longitude[lon]=lons[lon];
	}

	for (lat = 0; lat < NROW; lat++){
		d->lattitude[lat]=lats[lat];
	}

	/* Close the file. */
	if ((retval = nc_close(ncid)))
	ERR(retval);

	printf("*** SUCCESS reading file gw_data.nc!\n");

	

return 0;

}//end readGWData
