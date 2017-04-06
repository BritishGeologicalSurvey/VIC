
#include "GW_global_vars.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}//error statement for netcdf data
/******************************************************************************
* GW_write_output
* writes h data to netcdf file
*****************************************************************************/
int GW_write_output(const gw_global_data_struct *g, const gw_data_struct *d, const gw_param_struct *p, int ctime){

	/* This is the name of the data file we will read. */
	char name[20]="out_h";
	char file_name[50];
	int n;
	n=sprintf(file_name,"out_data_%d.nc",ctime);


	/* We are writing 2D data */
	size_t ndims= 2;
	char lat_name[20]= "Lat";
	char lon_name[20]= "Lon";

	/* Names of things. */
	char var1_name[20]= "h";
	//char var2_name[20]= "balance";
	char UNITS[20] ="units";


	/* For the units attributes. */
	char var1_units[20]= "m/day";
	//char var2_units[20]= "m/day";
	char lat_units[20]= "lon_units";
	char lon_units[20]= "lat_units";


	/* IDs for the netCDF file, dimensions, and variables. */
	int ncid, lon_dimid, lat_dimid, rec_dimid;
	int lat_varid, lon_varid, varid1, varid2;

	int dimids[ndims];

	/* The start and count arrays will tell the netCDF library where to
	write our data. */
	size_t start[ndims], count[ndims];

	/* Program variables to hold the data we will write out. We will only

	need enough space to hold one timestep of data; one record. */

	float var1_out[g->NCOL][g->NROW];

	//float var2_out[NROW][NCOL];

	float *p_var1_out;
	//float *p_var2_out;

	p_var1_out=&(var1_out[0][0]);
	//p_var2_out=&(var2_out[0][0]);

	/* These program variables hold the latitudes and longitudes.*/

	float lats[g->NROW], lons[g->NCOL];


	/* Loop indexes. */
	int lat, lon, rec, i = 0;
	int j;

	/* Error handling. */
	int retval;

	/*get lats and lons from gw_data_structure*/
	for (lat=0; lat<g->NROW; lat++){
		lats[lat]=(float)d->lattitude[lat];
	}
	for (lon=0; lon<g->NCOL; lon++){
		lons[lon]=(float)d->longitude[lon];
	}


	/* Create the file. */

	if ((retval = nc_create(file_name, NC_CLOBBER, &ncid)))
	ERR(retval);

	/* Define the dimensions. The record dimension is defined to have
	* unlimited length - it can grow as needed. In this example it is
	* the time dimension.*/
	if ((retval = nc_def_dim(ncid, lat_name, g->NROW, &lat_dimid)))
	ERR(retval);
	if ((retval = nc_def_dim(ncid, lon_name, g->NCOL, &lon_dimid)))
	ERR(retval);


	/* Define the coordinate variables. We will only define coordinate
	  variables for lat and lon. Ordinarily we would need to provide
	  an array of dimension IDs for each variable's dimensions, but
	  since coordinate variables only have one dimension, we can
	  simply provide the address of that dimension ID (&lat_dimid)and
	  similarly for (&lon_dimid). */
	if ((retval = nc_def_var(ncid, lon_name, NC_FLOAT, 1, &lon_dimid,
	&lon_varid)))
	ERR(retval);

	if ((retval = nc_def_var(ncid, lat_name, NC_FLOAT, 1, &lat_dimid,
	&lat_varid)))
	ERR(retval);

	 /* Assign units attributes to coordinate variables. */
	if ((retval = nc_put_att_text(ncid, lat_varid, UNITS,
	strlen(lon_units), lon_units)))
	ERR(retval);
	if ((retval = nc_put_att_text(ncid, lon_varid, UNITS,
	strlen(lat_units), lat_units)))
	ERR(retval);



	/* The dimids array is used to pass the dimids of the dimensions of
	the netCDF variables. Both of the netCDF variables we are
	creating share the same four dimensions. In C, the
	unlimited dimension must come first on the list of dimids. */

	dimids[1] = lat_dimid;
	dimids[0] = lon_dimid;

	 /* Define the netCDF variables for the data. */
	if ((retval = nc_def_var(ncid, var1_name, NC_FLOAT, ndims,
	dimids, &varid1)))
	ERR(retval);

	/* Assign units attributes to the netCDF variables. */
	if ((retval = nc_put_att_text(ncid, varid1, UNITS,
	strlen(var1_units), var1_units)))
	ERR(retval);

	/* End define mode. */

	if ((retval = nc_enddef(ncid)))
	ERR(retval);


	/* Write the coordinate variable data. This will put the lattitude
	and longitudes of our data grid into the netCDF file. */
	if ((retval = nc_put_var_float(ncid, lon_varid, &lons[0])))
	ERR(retval);
	if ((retval = nc_put_var_float(ncid, lat_varid, &lats[0])))
	ERR(retval);


	/* These settings tell netcdf to write one timestep of data.(The
	setting of start[0] inside the loop below tells netCDF which
	timestep to write.) */

	
	count[1] = g->NROW;
	count[0] = g->NCOL;
	start[0] = 0;
	start[1] = 0;
	//start[2] = 0;


	/* Write the  data. The arrays only hold one timestep worth
	of data.*/


	

	i=0;
	double temp;
				
	//get output variable into the right way
	for (lon=0; lon<g->NCOL; lon++){
			for(lat=0; lat<g->NROW;lat++){


					*(var1_out[0]+i)=p->h[lat][lon];//is this correct?

					i++;
				}
			}


		if ((retval = nc_put_vara_float(ncid, varid1, start, count,
			p_var1_out)))
			ERR(retval);

	 /* Close the file. */

	if ((retval = nc_close(ncid)))
		ERR(retval);

		printf("*** SUCCESS writing file %s!\n", file_name);
	return 0;
}//end creatOutNetcdfFile

