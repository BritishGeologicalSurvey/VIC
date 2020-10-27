
/******************************************************************************
* GW_read_1pumping
* function that reads initial heads
*****************************************************************************/
#include "GW_global_vars.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}//error statement for netcdf data
int GW_read_1pumping(const gw_global_data_struct *g, gw_data_struct *d, int year, int month, int day, int dayseconds){

	//printf("*** GW_read_1pumping is called \n");

	/* This is the name of the data file we will read. */
	char file_name[100];

	//printf("today is: %d. %d. %d\n", g->DAY,g->MONTH,g->YEAR);
	sprintf(file_name, "..//AMBHAS//gw_pumping_%d_%d_%d_%d.nc",  year,  month, day, dayseconds);
	printf("***pumping name %s \n", file_name);


	/* We are reading 2D data, a  -lat-lon grid. */
	size_t ndims= 2;
	char lat_name[20]= "Lat";
	char lon_name[20]= "Lon";


	/* Names of things. */
	char var1_name[20]= "pumping";


	char UNITS[20] ="units";

	/* For the units attributes. */
	char var1_units[20]= "m3/d";

	char lat_units[20]= "degrees_north";
	char lon_units[20]= "degrees_east";
	int ncid, varid1;
	int lat_varid, lon_varid;
	int count;


	/* The start and counter arrays will tell the netCDF library where to
	read our data. */
	size_t start[ndims], counter[ndims];

	/* Program variables to hold the data we will read. We will only
	need enough space to hold one timestep of data; one record. */
	float var1_in[g->NROW][g->NCOL];


	/*Output variables*/
	//float h[NROW][NCOL];

	/* These program variables hold the latitudes and longitudes. */
	float lats[g->NROW], lons[g->NCOL];

	/* Loop indexes. */
	int lvl, lat, lon, rec, i = 0;

	/* Error handling. */
	int retval;
	//printf("*** Flag0 \n");

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



	//printf("*** Flag4 \n");

	/* Read the data. Since we know the contents of the file we know
	* that the data arrays in this program are the correct size to
	* hold one timestep. */


	counter[0] = g->NROW; //orig
	counter[1] = g->NCOL;

	start[0] = 0;
	start[1] = 0;


	//printf("*** Flag5 \n");

	/* Read and check one record at a time. */
	//for (rec = 0; rec < NREC; rec++)
	//{
	if ((retval = nc_get_vara_float(ncid, varid1, start,
	counter, &var1_in[0][0])))
		ERR(retval);

	//printf("*** Flag6 \n");

	/* Check the data. */
	i = 0;


	for (lon = 0; lon < g->NCOL; lon++){
		for (lat = 0; lat < g->NROW; lat++){

      d->pumping[lat][lon]=*(var1_in[0]+i); 
			i++;
		}
	}
	//printf("*** Flag7 \n");
	/* Close the file. */
	if ((retval = nc_close(ncid)))
	ERR(retval);

	printf("*** SUCCESS reading %s.nc!\n",file_name);


	return 0;

}
