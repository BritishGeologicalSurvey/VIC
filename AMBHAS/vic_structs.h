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

double           ***out_data;
/******************************************************************************
 * @brief   Output Variable Types
 *****************************************************************************/
enum
{
    // Water Balance Terms - state variables
    OUT_ASAT,             /**< Saturated Area Fraction */
    OUT_LAKE_AREA_FRAC,   /**< lake surface area as fraction of the grid cell area [fraction] */
    OUT_LAKE_DEPTH,       /**< lake depth (distance between surface and deepest point) [m] */
    OUT_LAKE_ICE,         /**< moisture stored as lake ice [mm over lake ice area] */
    OUT_LAKE_ICE_FRACT,   /**< fractional coverage of lake ice [fraction] */
    OUT_LAKE_ICE_HEIGHT,  /**< thickness of lake ice [cm] */
    OUT_LAKE_MOIST,       /**< liquid water and ice stored in lake [mm over grid cell] */
    OUT_LAKE_SURF_AREA,   /**< lake surface area [m2] */
    OUT_LAKE_SWE,         /**< liquid water equivalent of snow on top of lake ice [m over lake ice area] */
    OUT_LAKE_SWE_V,       /**< volumetric liquid water equivalent of snow on top of lake ice [m3] */
    OUT_LAKE_VOLUME,      /**< lake volume [m3] */
    OUT_ROOTMOIST,        /**< root zone soil moisture  [mm] */
    OUT_SMFROZFRAC,       /**< fraction of soil moisture (by mass) that is ice, for each soil layer */
    OUT_SMLIQFRAC,        /**< fraction of soil moisture (by mass) that is liquid, for each soil layer */
    OUT_SNOW_CANOPY,      /**< snow interception storage in canopy  [mm] */
    OUT_SNOW_COVER,       /**< fractional area of snow cover [fraction] */
    OUT_SNOW_DEPTH,       /**< depth of snow pack [cm] */
    OUT_SOIL_ICE,         /**< soil ice content  [mm] for each soil layer */
    OUT_SOIL_LIQ,         /**< soil liquid content  [mm] for each soil layer */
    OUT_SOIL_ICE_FRAC,    /**< soil ice content fraction of column volume [1] for each soil layer */
    OUT_SOIL_LIQ_FRAC,    /**< soil liquid content fraction of column volume [1] for each soil layer */
    OUT_SOIL_MOIST,       /**< soil total moisture content  [mm] for each soil layer */
    OUT_SOIL_WET,         /**< vertical average of (soil moisture - wilting point)/(maximum soil moisture - wilting point) [mm/mm] */
    OUT_SURFSTOR,         /**< storage of liquid water and ice (not snow) on surface (ponding) [mm] */
    OUT_SURF_FROST_FRAC,  /**< fraction of soil surface that is frozen [fraction] */
    OUT_SWE,              /**< snow water equivalent in snow pack (including vegetation-intercepted snow)  [mm] */
    OUT_WDEW,             /**< total moisture interception storage in canopy [mm] */
    OUT_ZWT,              /**< water table position [cm] (zwt within lowest unsaturated layer) */
    OUT_ZWT_LUMPED,       /**< lumped water table position [cm] (zwt of total moisture across all layers, lumped together) */
    // Water Balance Terms - fluxes
    OUT_BASEFLOW,         /**< baseflow out of the bottom layer  [m/d] */
    OUT_RECHARGE,         /**< groundwater recharge out of the bottom layer  [mm] */
    OUT_DELINTERCEPT,     /**< change in canopy interception storage  [mm] */
    OUT_DELSOILMOIST,     /**< change in soil water content  [mm] */
    OUT_DELSURFSTOR,      /**< change in surface liquid water storage  [mm] */
    OUT_DELSWE,           /**< change in snow water equivalent  [mm] */
    OUT_EVAP,             /**< total net evaporation [mm] */
    OUT_EVAP_BARE,        /**< net evaporation from bare soil [mm] */
    OUT_EVAP_CANOP,       /**< net evaporation from canopy interception [mm] */
    OUT_INFLOW,           /**< moisture that reaches top of soil column [mm] */
    OUT_LAKE_BF_IN,       /**< incoming baseflow from lake catchment [mm] */
    OUT_LAKE_BF_IN_V,     /**< incoming volumetric baseflow from lake catchment [m3] */
    OUT_LAKE_BF_OUT,      /**< outgoing baseflow from lake [mm] */
    OUT_LAKE_BF_OUT_V,    /**< outgoing volumetric baseflow from lake [m3] */
    OUT_LAKE_CHAN_IN,     /**< channel inflow into lake [mm] */
    OUT_LAKE_CHAN_IN_V,   /**< volumetric channel inflow into lake [m3] */
    OUT_LAKE_CHAN_OUT,    /**< channel outflow from lake [mm] */
    OUT_LAKE_CHAN_OUT_V,  /**< volumetric channel outflow from lake [m3] */
    OUT_LAKE_DSTOR,       /**< change in lake moisture storage (liquid plus ice cover) [mm] */
    OUT_LAKE_DSTOR_V,     /**< volumetric change in lake moisture storage (liquid plus ice cover) [m3] */
    OUT_LAKE_DSWE,        /**< change in swe on top of lake ice [mm] */
    OUT_LAKE_DSWE_V,      /**< volumetric change in swe on top of lake ice [m3] */
    OUT_LAKE_EVAP,        /**< net evaporation from lake surface [mm] */
    OUT_LAKE_EVAP_V,      /**< net volumetric evaporation from lake surface [m3] */
    OUT_LAKE_PREC_V,      /**< volumetric precipitation over lake surface [m3] */
    OUT_LAKE_RCHRG,       /**< recharge from lake to surrounding wetland [mm] */
    OUT_LAKE_RCHRG_V,     /**< volumetric recharge from lake to surrounding wetland [m3] */
    OUT_LAKE_RO_IN,       /**< incoming runoff from lake catchment [mm] */
    OUT_LAKE_RO_IN_V,     /**< incoming volumetric runoff from lake catchment [m3] */
    OUT_LAKE_VAPFLX,      /**< outgoing sublimation from snow on top of lake ice [mm] */
    OUT_LAKE_VAPFLX_V,    /**< outgoing volumetric sublimation from snow on top of lake ice [m3] */
    OUT_PET,              /**< Potential evapotranspiration (= area-weighted sum of potential transpiration and potential soil evaporation).  Potential transpiration is computed using the Penman-Monteith eqn with architectural resistance and LAI of the current veg cover. [mm] */
    OUT_PREC,             /**< incoming precipitation [mm] */
    OUT_RAINF,            /**< rainfall  [mm] */
    OUT_REFREEZE,         /**< refreezing of water in the snow  [mm] */
    OUT_RUNOFF,           /**< surface runoff [mm] */
    OUT_SNOW_MELT,        /**< snow melt  [mm] */
    OUT_SNOWF,            /**< snowfall  [mm] */
    OUT_SUB_BLOWING,      /**< net sublimation of blowing snow [mm] */
    OUT_SUB_CANOP,        /**< net sublimation from snow stored in canopy [mm] */
    OUT_SUB_SNOW,         /**< total net sublimation from snow pack (surface and blowing) [mm] */
    OUT_SUB_SURFACE,      /**< net sublimation from snow pack surface [mm] */
    OUT_TRANSP_VEG,       /**< net transpiration from vegetation [mm] */
    OUT_WATER_ERROR,      /**< water budget error [mm] */
    // Energy Balance Terms - state variables
    OUT_ALBEDO,           /**< average surface albedo [fraction] */
    OUT_BARESOILT,        /**< bare soil surface temperature [C] */
    OUT_FDEPTH,           /**< depth of freezing fronts [cm] */
    OUT_LAKE_ICE_TEMP,    /**< temperature of lake ice [C] */
    OUT_LAKE_SURF_TEMP,   /**< lake surface temperature [C] */
    OUT_RAD_TEMP,         /**< average radiative surface temperature [K] */
    OUT_SALBEDO,          /**< snow pack albedo [fraction] */
    OUT_SNOW_PACK_TEMP,   /**< snow pack temperature [C] */
    OUT_SNOW_SURF_TEMP,   /**< snow surface temperature [C] */
    OUT_SNOWT_FBFLAG,     /**< snow surface temperature fallback flag */
    OUT_SOIL_TEMP,        /**< soil temperature [C] */
    OUT_SOIL_TNODE,       /**< soil temperature [C] */
    OUT_SOIL_TNODE_WL,    /**< soil temperature [C] */
    OUT_SOILT_FBFLAG,     /**< soil temperature flag for each soil thermal node */
    OUT_SURF_TEMP,        /**< average surface temperature [C] */
    OUT_SURFT_FBFLAG,     /**< surface temperature flag */
    OUT_TCAN_FBFLAG,      /**< Tcanopy flag */
    OUT_TDEPTH,           /**< depth of thawing fronts [cm] */
    OUT_TFOL_FBFLAG,      /**< Tfoliage flag */
    OUT_VEGT,             /**< average vegetation canopy temperature [C] */
    // Energy Balance Terms - fluxes
    OUT_ADV_SENS,         /**< net sensible flux advected to snow pack [W/m2] */
    OUT_ADVECTION,        /**< advected energy [W/m2] */
    OUT_DELTACC,          /**< rate of change in cold content in snow pack [W/m2] */
    OUT_DELTAH,           /**< rate of change in heat storage [W/m2] */
    OUT_ENERGY_ERROR,     /**< energy budget error [W/m2] */
    OUT_FUSION,           /**< net energy used to melt/freeze soil moisture [W/m2] */
    OUT_GRND_FLUX,        /**< net heat flux into ground [W/m2] */
    OUT_IN_LONG,          /**< incoming longwave at ground surface (under veg) [W/m2] */
    OUT_LATENT,           /**< net upward latent heat flux [W/m2] */
    OUT_LATENT_SUB,       /**< net upward latent heat flux from sublimation [W/m2] */
    OUT_MELT_ENERGY,      /**< energy of fusion (melting) in snowpack [W/m2] */
    OUT_LWNET,            /**< net downward longwave flux [W/m2] */
    OUT_SWNET,            /**< net downward shortwave flux [W/m2] */
    OUT_R_NET,            /**< net downward radiation flux [W/m2] */
    OUT_RFRZ_ENERGY,      /**< net energy used to refreeze liquid water in snowpack [W/m2] */
    OUT_SENSIBLE,         /**< net upward sensible heat flux [W/m2] */
    OUT_SNOW_FLUX,        /**< energy flux through snow pack [W/m2] */
    // Miscellaneous Terms
    OUT_AERO_COND,        /**< "scene" aerodynamic conductance [m/s] (tiles with overstory contribute overstory conductance; others contribute surface conductance) */
    OUT_AERO_COND1,       /**< surface aerodynamic conductance [m/s] */
    OUT_AERO_COND2,       /**< overstory aerodynamic conductance [m/s] */
    OUT_AERO_RESIST,      /**< "scene"canopy aerodynamic resistance [s/m]  (tiles with overstory contribute overstory resistance; others contribute surface resistance)*/
    OUT_AERO_RESIST1,     /**< surface aerodynamic resistance [s/m] */
    OUT_AERO_RESIST2,     /**< overstory aerodynamic resistance [s/m] */
    OUT_AIR_TEMP,         /**< air temperature [C] */
    OUT_CATM,             /**< atmospheric CO2 concentrtaion [ppm]*/
    OUT_DENSITY,          /**< near-surface atmospheric density [kg/m3]*/
    OUT_FCANOPY,          /**< fractional area covered by plant canopy [fraction] */
    OUT_FDIR,             /**< fraction of incoming shortwave that is direct [fraction]*/
    OUT_LAI,              /**< leaf area index [m2/m2] */
    OUT_LWDOWN,           /**< incoming longwave [W/m2] */
    OUT_PAR,              /**< incoming photosynthetically active radiation [W/m2] */
    OUT_PRESSURE,         /**< near surface atmospheric pressure [kPa] */
    OUT_QAIR,             /**< specific humidity [kg/kg] */
    OUT_REL_HUMID,        /**< relative humidity [%]*/
    OUT_SWDOWN,           /**< incoming shortwave [W/m2] */
    OUT_SURF_COND,        /**< surface conductance [m/s] */
    OUT_VP,               /**< near surface vapor pressure [kPa] */
    OUT_VPD,              /**< near surface vapor pressure deficit [kPa] */
    OUT_WIND,             /**< near surface wind speed [m/s] */
    // Band-specific quantities
    OUT_ADV_SENS_BAND,    /**< net sensible heat flux advected to snow pack [W/m2] */
    OUT_ADVECTION_BAND,   /**< advected energy [W/m2] */
    OUT_ALBEDO_BAND,      /**< average surface albedo [fraction] */
    OUT_DELTACC_BAND,     /**< change in cold content in snow pack [W/m2] */
    OUT_GRND_FLUX_BAND,   /**< net heat flux into ground [W/m2] */
    OUT_IN_LONG_BAND,     /**< incoming longwave at ground surface (under veg) [W/m2] */
    OUT_LATENT_BAND,      /**< net upward latent heat flux [W/m2] */
    OUT_LATENT_SUB_BAND,  /**< net upward latent heat flux due to sublimation [W/m2] */
    OUT_MELT_ENERGY_BAND, /**< energy of fusion (melting) in snowpack [W/m2] */
    OUT_LWNET_BAND,       /**< net downward longwave flux [W/m2] */
    OUT_SWNET_BAND,       /**< net downward shortwave flux [W/m2] */
    OUT_RFRZ_ENERGY_BAND, /**< net energy used to refreeze liquid water in snowpack [W/m2] */
    OUT_SENSIBLE_BAND,    /**< net upward sensible heat flux [W/m2] */
    OUT_SNOW_CANOPY_BAND, /**< snow interception storage in canopy [mm] */
    OUT_SNOW_COVER_BAND,  /**< fractional area of snow cover [fraction] */
    OUT_SNOW_DEPTH_BAND,  /**< depth of snow pack [cm] */
    OUT_SNOW_FLUX_BAND,   /**< energy flux through snow pack [W/m2] */
    OUT_SNOW_MELT_BAND,   /**< snow melt [mm] */
    OUT_SNOW_PACKT_BAND,  /**< snow pack temperature [C] */
    OUT_SNOW_SURFT_BAND,  /**< snow surface temperature [C] */
    OUT_SWE_BAND,         /**< snow water equivalent in snow pack [mm] */
    // Carbon-Cycling Terms
    OUT_APAR,             /**< absorbed PAR [W/m2] */
    OUT_GPP,              /**< gross primary productivity [g C/m2d] */
    OUT_RAUT,             /**< autotrophic respiration [g C/m2d] */
    OUT_NPP,              /**< net primary productivity [g C/m2d] */
    OUT_LITTERFALL,       /**< flux of carbon from living biomass into soil [g C/m2d] */
    OUT_RHET,             /**< soil respiration (heterotrophic respiration) [g C/m2d] */
    OUT_NEE,              /**< net ecosystem exchange (=NPP-RHET) [g C/m2d] */
    OUT_CLITTER,          /**< Carbon density in litter pool [g C/m2] */
    OUT_CINTER,           /**< Carbon density in intermediate pool [g C/m2] */
    OUT_CSLOW,            /**< Carbon density in slow pool [g C/m2] */
    // Timing and Profiling Terms
    OUT_TIME_VICRUN_WALL, /**< Wall time spent inside vic_run [seconds] */
    OUT_TIME_VICRUN_CPU,  /**< Wall time spent inside vic_run [seconds] */
    // Last value of enum - DO NOT ADD ANYTHING BELOW THIS LINE!!
    // used as a loop counter and must be >= the largest value in this enum
    N_OUTVAR_TYPES        /**< used as a loop counter*/
};


/******************************************************************************
 * @brief    Structure to store location information for individual grid cells.
 * @details  The global and local indices show the position of the grid cell
 *           within the global and local (processor) domains. When the model is
 *           run on a single processor, the glonal and local domains are
 *           identical. The model is run over a list of cells.
 *****************************************************************************/
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

/******************************************************************************
 * @brief    Structure for netcdf variable information
 *****************************************************************************/
typedef struct {
    int nc_varid;                   /**< variable netcdf id */
    int nc_type;                    /**< variable netcdf type */
    int nc_dimids[MAXDIMS];         /**< ids of dimensions */
    int nc_counts[MAXDIMS];      /**< size of dimid */
    int nc_dims;                 /**< number of dimensions */
} nc_var_struct;

/******************************************************************************
 * @brief    Structure for netcdf file information. Initially to store
 *           information for the output files (state and history)
 *****************************************************************************/
typedef struct {
    char c_fillvalue;
    int i_fillvalue;
    double d_fillvalue;
    float f_fillvalue;
    short int s_fillvalue;
    int nc_id;
    int band_dimid;
    int front_dimid;
    int frost_dimid;
    int lake_node_dimid;
    int layer_dimid;
    int ni_dimid;
    int nj_dimid;
    int node_dimid;
    int root_zone_dimid;
    int time_dimid;
    int time_bounds_dimid;
    int veg_dimid;
    int time_varid;
    int time_bounds_varid;
    int band_size;
    int front_size;
    int frost_size;
    int lake_node_size;
    int layer_size;
    int ni_size;
    int nj_size;
    int node_size;
    int root_zone_size;
    int time_size;
    int veg_size;
    int open;
    nc_var_struct *nc_vars;
} nc_file_struct;

/******************************************************************************
 * @brief    Structure for mapping the vegetation types for each grid cell as
 *           stored in VIC's veg_con_struct to a regular array.
 *****************************************************************************/
typedef struct {
    int nv_types; /**< total number of vegetation types */
                     /**< size of vidx and Cv arrays */
    int nv_active; /**< number of active vegetation types. Because of the */
                      /**< way that VIC defines nveg, this is nveg+1 */
                      /**< (for bare soil) or nveg+2 (if the treeline option */
                      /**< is active as well) */
    int *vidx;     /**< array of indices for active vegetation types */
    double *Cv;    /**< array of fractional coverage for nc_types */
} veg_con_map_struct;
/******************************************************************************
 * @brief   This structure stores soil variables for the complete soil column
 *          for each grid cell.
 *****************************************************************************/
typedef struct {
    // State variables
    double aero_resist[2];             /**< The (stability-corrected) aerodynamic
                                          resistance (s/m) that was actually used
                                          in flux calculations.
                                          [0] = surface (bare soil, non-overstory veg, or snow pack)
                                          [1] = overstory */
    double asat;                       /**< saturated area fraction */
    double CLitter;                    /**< carbon storage in litter pool [gC/m2] */
    double CInter;                     /**< carbon storage in intermediate pool [gC/m2] */
    double CSlow;                      /**< carbon storage in slow pool [gC/m2] */
    //layer_data_struct layer[MAX_LAYERS]; 
					/**< structure containing soil variables
                                            for each layer (see above; including both
                                            state and flux variables) */
    double rootmoist;                  /**< total of layer.moist over all layers
                                          in the root zone (mm) */
    double wetness;                    /**< average of
                                          (layer.moist - Wpwp)/(porosity*depth - Wpwp)
                                          over all layers (fraction) */
    double zwt;                        /**< average water table position [cm] - using lowest unsaturated layer */
    double zwt_lumped;                 /**< average water table position [cm] - lumping all layers' moisture together */

    // Fluxes
    double pot_evap;                   /**< potential evaporation (mm) */
    double baseflow;                   /**< baseflow from current cell (mm/TS) */
    double recharge;                   /**< recharge from current cell (m/TS) */
    double runoff;                     /**< runoff from current cell (mm/TS) */
    double inflow;                     /**< moisture that reaches the top of
                                          the soil column (mm) */
    double RhLitter;                   /**< soil respiration from litter pool [gC/m2] */
    double RhLitter2Atm;               /**< soil respiration from litter pool [gC/m2] that goes to atmosphere */
    double RhInter;                    /**< soil respiration from intermediate pool [gC/m2] */
    double RhSlow;                     /**< soil respiration from slow pool [gC/m2] */
    double RhTot;                      /**< total soil respiration over all pools [gC/m2] (=RhLitter2Atm+RhInter+RhSlow) */
} cell_data_struct;
/******************************************************************************
 * @brief   This structure stores all variables needed to solve, or save
 *          solututions for all versions of this model. only cell data stored here
 *****************************************************************************/
typedef struct {
    cell_data_struct **cell;      /**< Stores soil layer variables */

} all_vars_struct;



