/******************************************************************************
 * @ JUST FOR TESTING!!! size_t and bool data types are replaced with int
 *

 *****************************************************************************/
#define MAXSTRING 30
#define MAXDIMS 30
//#include "vic_driver_shared_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef struct {
    int run; /**< TRUE: run grid cell. FALSE: do not run grid cell */
    double latitude; /**< latitude of grid cell center */
    double longitude; /**< longitude of grid cell center */
    double area; /**< area of grid cell */
    double frac; /**< fraction of grid cell that is active */
    int nveg; /**< number of vegetation type according to parameter file */
    int global_idx; /**< index of grid cell in global list of grid cells */
    int io_idx; /**< index of cell in 1-D I/O arrays */
    int local_idx; /**< index of grid cell in local list of grid cells */
    size_t A_nlat; /**< counter in AMBHAS grid in lat or crow */
    size_t A_nlon; /**< counter in AMBHAS grid in lon or ccol */
    double Kaq;	/**< aquifer hydraulic conductivity [m/day] from AMBHAS **/
    double z;	/**< depth to the water table: DEM-hydraulic head from AMBHAS **/
} location_struct;

/******************************************************************************
 * @brief    Structure to store information about the domain file.
 *****************************************************************************/
typedef struct {
    char lat_var[MAXSTRING]; /**< latitude variable name in the domain file */
    char lon_var[MAXSTRING];  /**< longitude variable name in the domain file */
    char mask_var[MAXSTRING]; /**< mask variable name in the domain file */
    char area_var[MAXSTRING]; /**< area variable name in the domain file */
    char frac_var[MAXSTRING]; /**< fraction variable name in the domain file */
    char y_dim[MAXSTRING]; /**< y dimension name in the domain file */
    char x_dim[MAXSTRING]; /**< x dimension name in the domain file */
    int n_coord_dims; /**< number of x/y coordinates */
} domain_info_struct;

/******************************************************************************
 * @brief    Structure to store local and global domain information. If the
 *           model is run on a single processor, then the two are identical.
 *****************************************************************************/
typedef struct {
    int ncells_total; /**< total number of grid cells on domain */
    int ncells_active; /**< number of active grid cells on domain */
    int n_nx; /**< size of x-index; */
    int n_ny; /**< size of y-index */
    location_struct *locations; /**< locations structs for local domain */
    domain_info_struct info; /**< structure storing domain file info */
} domain_struct;
