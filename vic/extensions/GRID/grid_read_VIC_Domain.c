
/******************************************************************************
* GW_read_h_ini
* function that reads initial heads
*****************************************************************************/
#include "Grid_global_vars.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include "netcdf.h"
#include <string.h>
#include <math.h>
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return 2;}//error statement for netcdf data
int grid_read_VIC_Domain(grid_data_struct *grid){

printf("*** GW_read_VIC_Domain is called \n");
printf("grid_read_VIC_Domain: NROW_VIC %d, NCOL_VIC %d, NROW_FORCE %d, NCOL_FORCE %d \n", grid->NROW_VIC, grid->NCOL_VIC, grid->NROW_FORCE, grid->NCOL_FORCE);

	/* This is the name of the data file we will read. */
	char file_name[100]= "..//grid//";
	strcat(file_name, grid->VIC_name);
	printf("*** %s \n", file_name);

	/* We are reading 2D data, a  -lat-lon grid. */
	size_t ndims= 2;
	char lat_name[20]= "Lat";
	char lon_name[20]= "Lon";
	//printf("*** Flag0 \n");

	/* Names of things. */
	char var1_name[20]= "cell";

	//printf("*** Flag0 \n");
	char UNITS[20] ="units";

	/* For the units attributes. */
	char var1_units[20]= "-";

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
	float var1_in[grid->NROW_VIC][grid->NCOL_VIC];

//	printf("*** Flag0 \n");
	/*Output variables*/
	//float h[NROW][NCOL];

	/* These program variables hold the latitudes and longitudes. */
	float lats[grid->NROW_VIC],  lons[grid->NCOL_VIC];

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



	//printf("*** Flag4 \n");

	/* Read the data. Since we know the contents of the file we know
	* that the data arrays in this program are the correct size to
	* hold one timestep. */


	counter[0] = grid->NROW_VIC; //orig
	counter[1] = grid->NCOL_VIC;


	start[0] = 0;
	start[1] = 0;


	//printf("*** Flag5 \n");

	if ((retval = nc_get_vara_float(ncid, varid1, start,
	counter, &var1_in[0][0])))
		ERR(retval);

	//printf("*** Flag6 \n");

	/* Check the data. */
	i = 0;

	for (lat = 0; lat < grid->NROW_VIC; lat++){
		for (lon = 0; lon < grid->NCOL_VIC; lon++){
			grid->cell_no_VIC[lat][lon]=*(var1_in[0]+i);
			i++;
		}
	}
 
/*
 for(int i=0; i < grid->NROW_VIC* grid->NCOL_VIC; i++){
    lat = i/grid->NCOL_VIC;
    lon = i % grid->NCOL_VIC -1; 
    printf("cell_no_VIC %f, var1_in %f \n", grid->cell_no_VIC[lat][lon], *(var1_in[0]+i));
 }
 */
 i=0;
 int lat_calc, lon_calc;
	for (lat = 0; lat < grid->NROW_VIC; lat++){
		for (lon = 0; lon < grid->NCOL_VIC; lon++){   
       lat_calc = i/grid->NCOL_VIC;
       lon_calc = i%grid->NCOL_VIC; 
      // printf("cell_no_VIC %f, var1_in %f lat %d lat_calc %d lon %d, lon_calc %d \n", grid->cell_no_VIC[lat][lon], *(var1_in[0]+i), lat, lat_calc, lon, lon_calc);
       i++;
    }
  }
      
   
   
 
/* 	//Write lats to variable 
 	printf("\n*** Lats VIC!\n");
 	for (lat = 0; lat < grid->NROW_VIC; lat++){
   	grid->lat_VIC[lat] = lats[lat];
    printf(" %f ", lats[lat]);
   }
	//Write lons to variable 
 	printf("\n*** Lons VIC!\n");
 	for (lon = 0; lon < grid->NCOL_VIC; lon++){
   grid->lon_VIC[lon] = lons[lon];
   printf(" %f ", lons[lon]);   
   }
   */
	//printf("*** Flag7 \n");
	/* Close the file. */
	if ((retval = nc_close(ncid)))
	ERR(retval);

	printf("*** SUCCESS reading file VIC_cell file !\n");

	/*
	printf("hini from input file: \n");
	for (lat=0; lat<g->NROW; lat++){
		for (lon=0; lon<g->NCOL; lon ++){
			printf(" %.1f ",p->h[lat][lon]);
			count++;
		}
		printf("\n");
	}
	*/



return 0;

}
