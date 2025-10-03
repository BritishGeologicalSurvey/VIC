/******************************************************************************
* GW_read_data (heap-allocated buffers)
* Reads netCDF data prepared in R, transforms it and writes into gw_data_struct
*****************************************************************************/
#include "GW_global_vars.h"
#include "netcdf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Note: We don't use the old ERR macro because we need to clean up heap memory. */

int GW_read_data(const gw_global_data_struct *g, gw_data_struct *d){

    /* Build filename */
    char file_name[500] = "..//AMBHAS//";
    strcat(file_name, g->gwname);
    printf("*** %s \n", file_name);

    /* Dimensions */
    size_t ndims = 2;
    char lat_name[20] = "Lat";
    char lon_name[20] = "Lon";
    int NROW = g->NROW;
    int NCOL = g->NCOL;
    printf("NROW %d \n", NROW);
    printf("NCOL %d\n", NCOL);

    /* Variable names */
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


    printf("File Name before opening file = %s \n", file_name);

    int ncid = -1;
    int varid1, varid2, varid3, varid4, varid5, varid6, varid7, varid8, varid9, varid10, varid11, varid12, varid13, varid14, varid15;
    int varid16= -1, varid17= -1, varid18= -1, varid19 = -1, varid20 = -1;
    int varid21 = -1, varid22 = -1;
    int lat_varid, lon_varid;
    int retval;

    printf("still running 1 \n");

    size_t start[2], counter[2];

    printf("still running 2 \n");

    /* Total number of grid cells (use size_t to avoid overflow) */
    size_t total = (size_t)NROW * (size_t)NCOL;
    if (total == 0) {
        printf("Error: NROW or NCOL is zero\n");
        return 2;
    }

    /* Pointers for variable buffers (linear arrays of length total) */
    float *var1_in = NULL, *var2_in = NULL, *var3_in = NULL, *var4_in = NULL, *var5_in = NULL;
    float *var6_in = NULL, *var7_in = NULL, *var8_in = NULL, *var9_in = NULL, *var10_in = NULL;
    float *var11_in = NULL, *var12_in = NULL, *var13_in = NULL, *var14_in = NULL, *var15_in = NULL;
    float *var16_in = NULL, *var17_in = NULL, *var18_in = NULL, *var19_in = NULL, *var20_in = NULL;
    float *var21_in = NULL, *var22_in = NULL;

    /* Lat/lon arrays */
    float *lats = NULL, *lons = NULL;

   

    /* Allocate arrays that are always used: var1..var15, lats, lons */
    #define ALLOC_OR_FAIL(ptr, cnt) do { \
        (ptr) = (float *) malloc((size_t)(cnt) * sizeof(float)); \
        if ((ptr) == NULL) { \
            printf("Error: malloc failed for %s\n", #ptr); \
            goto cleanup; \
        } \
    } while(0)

    ALLOC_OR_FAIL(var1_in, total);
    ALLOC_OR_FAIL(var2_in, total);
    ALLOC_OR_FAIL(var3_in, total);
    ALLOC_OR_FAIL(var4_in, total);
    ALLOC_OR_FAIL(var5_in, total);
    ALLOC_OR_FAIL(var6_in, total);
    ALLOC_OR_FAIL(var7_in, total);
    ALLOC_OR_FAIL(var8_in, total);
    ALLOC_OR_FAIL(var9_in, total);
    ALLOC_OR_FAIL(var10_in, total);
    ALLOC_OR_FAIL(var11_in, total);
    ALLOC_OR_FAIL(var12_in, total);
    ALLOC_OR_FAIL(var13_in, total);
    ALLOC_OR_FAIL(var14_in, total);
    ALLOC_OR_FAIL(var15_in, total);

    ALLOC_OR_FAIL(lats, (size_t)NROW);
    ALLOC_OR_FAIL(lons, (size_t)NCOL);

    /* Conditionally allocate the optional arrays */
    if (g->GRID == 1) {
        ALLOC_OR_FAIL(var16_in, total);
        ALLOC_OR_FAIL(var17_in, total);
        ALLOC_OR_FAIL(var18_in, total);
        ALLOC_OR_FAIL(var19_in, total);
        ALLOC_OR_FAIL(var20_in, total);
    }
    if (g->VARIABLE_SY == 1) {
        ALLOC_OR_FAIL(var21_in, total);
        ALLOC_OR_FAIL(var22_in, total);
    }


    #undef ALLOC_OR_FAIL


    /* Open the file. */
    if ((retval = nc_open(file_name, NC_NOWRITE, &ncid))) {
        printf("Error: nc_open failed: %s\n", nc_strerror(retval));
        goto cleanup;
    }

    /* Get lat/lon varids */
    if ((retval = nc_inq_varid(ncid, lat_name, &lat_varid))) {
        printf("Error: nc_inq_varid lat: %s\n", nc_strerror(retval));
        goto cleanup;
    }
    if ((retval = nc_inq_varid(ncid, lon_name, &lon_varid))) {
        printf("Error: nc_inq_varid lon: %s\n", nc_strerror(retval));
        goto cleanup;
    }

    /* Read lat/lon */
    if ((retval = nc_get_var_float(ncid, lat_varid, lats))) {
        printf("Error: nc_get_var_float lat: %s\n", nc_strerror(retval));
        goto cleanup;
    }
    if ((retval = nc_get_var_float(ncid, lon_varid, lons))) {
        printf("Error: nc_get_var_float lon: %s\n", nc_strerror(retval));
        goto cleanup;
    }

    /* Get varids for required variables (1..15) */
    if ((retval = nc_inq_varid(ncid, var1_name, &varid1))) { printf("Error var1: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var2_name, &varid2))) { printf("Error var2: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var3_name, &varid3))) { printf("Error var3: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var4_name, &varid4))) { printf("Error var4: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var5_name, &varid5))) { printf("Error var5: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var6_name, &varid6))) { printf("Error var6: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var7_name, &varid7))) { printf("Error var7: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var8_name, &varid8))) { printf("Error var8: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var9_name, &varid9))) { printf("Error var9: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var10_name, &varid10))) { printf("Error var10: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var11_name, &varid11))) { printf("Error var11: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var12_name, &varid12))) { printf("Error var12: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var13_name, &varid13))) { printf("Error var13: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var14_name, &varid14))) { printf("Error var14: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_inq_varid(ncid, var15_name, &varid15))) { printf("Error var15: %s\n", nc_strerror(retval)); goto cleanup; }

    /* Optional varids */
    if (g->GRID == 1) {
        if ((retval = nc_inq_varid(ncid, var16_name, &varid16))) { printf("Error var16: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_inq_varid(ncid, var17_name, &varid17))) { printf("Error var17: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_inq_varid(ncid, var18_name, &varid18))) { printf("Error var18: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_inq_varid(ncid, var19_name, &varid19))) { printf("Error var19: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_inq_varid(ncid, var20_name, &varid20))) { printf("Error var20: %s\n", nc_strerror(retval)); goto cleanup; }
    }
    if (g->VARIABLE_SY == 1) {
        if ((retval = nc_inq_varid(ncid, var21_name, &varid21))) { printf("Error var21: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_inq_varid(ncid, var22_name, &varid22))) { printf("Error var22: %s\n", nc_strerror(retval)); goto cleanup; }
    }


    /* Prepare start/counter and read arrays. */
    counter[0] = (size_t) NROW;
    counter[1] = (size_t) NCOL;
    start[0] = 0;
    start[1] = 0;

    if ((retval = nc_get_vara_float(ncid, varid1, start, counter, var1_in))) { printf("Error read var1: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid2, start, counter, var2_in))) { printf("Error read var2: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid3, start, counter, var3_in))) { printf("Error read var3: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid4, start, counter, var4_in))) { printf("Error read var4: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid5, start, counter, var5_in))) { printf("Error read var5: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid6, start, counter, var6_in))) { printf("Error read var6: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid7, start, counter, var7_in))) { printf("Error read var7: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid8, start, counter, var8_in))) { printf("Error read var8: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid9, start, counter, var9_in))) { printf("Error read var9: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid10, start, counter, var10_in))) { printf("Error read var10: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid11, start, counter, var11_in))) { printf("Error read var11: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid12, start, counter, var12_in))) { printf("Error read var12: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid13, start, counter, var13_in))) { printf("Error read var13: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid14, start, counter, var14_in))) { printf("Error read var14: %s\n", nc_strerror(retval)); goto cleanup; }
    if ((retval = nc_get_vara_float(ncid, varid15, start, counter, var15_in))) { printf("Error read var15: %s\n", nc_strerror(retval)); goto cleanup; }

    if (g->GRID == 1) {
        if ((retval = nc_get_vara_float(ncid, varid16, start, counter, var16_in))) { printf("Error read var16: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_get_vara_float(ncid, varid17, start, counter, var17_in))) { printf("Error read var17: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_get_vara_float(ncid, varid18, start, counter, var18_in))) { printf("Error read var18: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_get_vara_float(ncid, varid19, start, counter, var19_in))) { printf("Error read var19: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_get_vara_float(ncid, varid20, start, counter, var20_in))) { printf("Error read var20: %s\n", nc_strerror(retval)); goto cleanup; }
    }
    if (g->VARIABLE_SY == 1) {
        if ((retval = nc_get_vara_float(ncid, varid21, start, counter, var21_in))) { printf("Error read var21: %s\n", nc_strerror(retval)); goto cleanup; }
        if ((retval = nc_get_vara_float(ncid, varid22, start, counter, var22_in))) { printf("Error read var22: %s\n", nc_strerror(retval)); goto cleanup; }
    }


    /* Fill the gw_data_struct arrays. The original code used a linear index 'i' */
    size_t i = 0;
    for (int lat = 0; lat < NROW; lat++) {
        for (int lon = 0; lon < NCOL; lon++) {
            /* linear index i corresponds to original *(varX_in[0] + i) */
            d->Sy_aq[lat][lon]      = var1_in[i];
            d->Trans[lat][lon]     = var2_in[i];
            d->K[lat][lon]         = var3_in[i];
            d->mask[lat][lon]      = var4_in[i];
            d->dem[lat][lon]       = var5_in[i];
            d->zbase[lat][lon]     = var6_in[i];
            d->zriver[lat][lon]    = var7_in[i];
            d->driver[lat][lon]    = var8_in[i];
            d->C_eff[lat][lon]     = var9_in[i];
            d->C_in[lat][lon]      = var10_in[i];
            d->C_leak_eff[lat][lon]= var11_in[i];
            d->C_leak_in[lat][lon] = var12_in[i];
            d->headBC[lat][lon]    = var13_in[i];
            d->river_area[lat][lon]= var14_in[i];
            d->aquiferMap[lat][lon]= var15_in[i];

            if (g->GRID == 1) {
                d->c_n[lat][lon] = var16_in[i];
                d->c_e[lat][lon] = var17_in[i];
                d->e_n[lat][lon] = var18_in[i];
                d->e_e[lat][lon] = var19_in[i];
                d->area[lat][lon]= var20_in[i];
            }
            if (g->VARIABLE_SY == 1) {
                d->z_soil[lat][lon]  = var21_in[i];
                d->Sy_soil[lat][lon] = var22_in[i];
            }

            i++;
        }
    }

    /* Fill lat/lon arrays in d (same rounding as original code) */
    for (int lon = 0; lon < NCOL; lon++) {
        d->longitude[lon] = (int)(10000*lons[lon]) / 10000.0;
    }
    for (int lat = 0; lat < NROW; lat++) {
        d->lattitude[lat] = (int)(10000*lats[lat]) / 10000.0;
    }

    /* Close file */
    if ((retval = nc_close(ncid))) {
        printf("Error closing file: %s\n", nc_strerror(retval));
        /* still continue to free memory and return error */
        goto cleanup;
    }
    ncid = -1; /* mark closed */

    printf("*** SUCCESS reading file %s!\n", file_name);

    /* Normal cleanup and return success */
    retval = 0;
cleanup:
    /* If file still open, try to close it */
    if (ncid >= 0) {
        int tmp = nc_close(ncid);
        if (tmp) printf("Warning: nc_close on cleanup failed: %s\n", nc_strerror(tmp));
    }

    /* free all allocated buffers (check for NULL) */
    free(var1_in); free(var2_in); free(var3_in); free(var4_in); free(var5_in);
    free(var6_in); free(var7_in); free(var8_in); free(var9_in); free(var10_in);
    free(var11_in); free(var12_in); free(var13_in); free(var14_in); free(var15_in);
    free(var16_in); free(var17_in); free(var18_in); free(var19_in); free(var20_in);
    free(var21_in); free(var22_in);
    free(lats); free(lons);

    return retval ? 2 : 0;
} /* end GW_read_data */
