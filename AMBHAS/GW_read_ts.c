#include "GW_global_vars.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}//error statement for netcdf data
/******************************************************************************
* GW_read_Ts
*****************************************************************************/
int GW_read_Ts(const gw_global_data_struct *g, gw_data_struct *d, int ctime){


	/* This is the name of the data file we will read. */
	char file_name[20]= "gw_ts.nc";

	/* We are reading 3D data, a  11 x 10 lvl-lat-lon grid, with 2
	timesteps of data. */
	size_t ndims= 3;
	char lat_name[20]= "Lat";
	char lon_name[20]= "Lon";
	char rec_name[20]= "Time";


	/* Names of things. */
	//char var1_name[20]= "recharge";
	char var1_name[20]= "pumping";


	char UNITS[20] ="units";

	/* For the units attributes. */
	//#define UNITS "units"
	char var1_units[20]= "m3/day";
	//char var2_units[20]= "m3/day";
	char lat_units[20]= "degrees_north";
	char lon_units[20]= "degrees_east";

	int ncid, varid1, varid2;
	int lat_varid, lon_varid;

	/* The start and count arrays will tell the netCDF library where to
	read our data. */
	size_t start[ndims], count[ndims];

	/* Program variables to hold the data we will read. We will only
	need enough space to hold one timestep of data; one record. */
	float var1_in[g->NROW][g->NCOL];
	//float var2_in[g->NROW][g->NCOL];

	/* These program variables hold the latitudes and longitudes. */
	float lats[g->NROW], lons[g->NCOL];

	/* Loop indexes. */
	int lat, lon, rec, i = 0, j=0;

	/* Error handling. */
	int retval;

	/* Open the file. */
	if ((retval = nc_open(file_name, NC_NOWRITE, &ncid)))
	ERR(retval);

	//printf("*** Flag1\n");

	/* Get the varids of the latitude and longitude coordinate
	* variables. */
	if ((retval = nc_inq_varid(ncid, lat_name, &lat_varid)))
	ERR(retval);
	if ((retval = nc_inq_varid(ncid, lon_name, &lon_varid)))
	ERR(retval);

	//printf("*** Flag2\n");

	/* Read the coordinate variable data. */
	if ((retval = nc_get_var_float(ncid, lat_varid, &lats[0])))
	ERR(retval);
	if ((retval = nc_get_var_float(ncid, lon_varid, &lons[0])))
	ERR(retval);

	//printf("*** Flag3\n");

	/* Get the varids of the pressure and temperature netCDF
	* variables. */
	if ((retval = nc_inq_varid(ncid, var1_name, &varid1)))
	ERR(retval);
	/*if ((retval = nc_inq_varid(ncid, var2_name, &varid2)))
	ERR(retval);*/

	//printf("*** Flag4\n");

	/* Read the data. Since we know the contents of the file we know
	* that the data arrays in this program are the correct size to
	* hold one timestep. */

	count[0] = 1;
	count[1] = g->NCOL;
	count[2] = g->NROW;

	start[0] = 0;
	start[1] = 0;
	start[2] = 0;



	/* Read and check one record at a time. */
	for (rec = ctime; rec < ctime+1; rec++){
		start[0] = rec;
		if ((retval = nc_get_vara_float(ncid, varid1, start,
		count, &var1_in[0][0])))
		ERR(retval);
		/*if ((retval = nc_get_vara_float(ncid, varid2, start,
		count, &var2_in[0][0])))
		ERR(retval);*/

	i = 0;
	for (lon = 0; lon < g->NCOL; lon++){
	
		for (lat = 0; lat < g->NROW; lat++){

			
			//Store data in gw_ts structure
			d->pumping[lat][lon]=*(var1_in[0]+i); 
			//d->recharge[lat][lon]=*(var1_in[0]+i); 
			i++;

			}
			
		}


	} /* next record */

	/* Close the file. */
	if ((retval = nc_close(ncid)))
	ERR(retval);

	//printf("*** SUCCESS reading file gw_ts.nc!\n");

	return 0;
}//end readGwTs

