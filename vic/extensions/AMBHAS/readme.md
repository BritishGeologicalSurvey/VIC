VIC-AMBHAS-GRID User Manual

## Introduction

This documents describes the input files for the coupled VIC-AMBHAS-GRID model. AMBHAS is a 2D lateral groundwater model and is built into the image version of VIC 5.0.1. VIC calculates a groundwater recharge as a function of soil moisture and water table depth and passes this into AMBHAS. AMBHAS calculates lateral groundwater flow, groundwater baseflow and interacts with the land surface. AMBHAS feeds back the water table depth and groundwater baseflow to VIC. There is also an option of abstracting groundwater from the aquifer, specified through model input. The model can be run in a dynamic steady state, using one year of forcing data or as time-variant. 
This version of VIC will only work as the coupled version of VIC-AMBHAS-GRID, and only with the image version of VIC.
To run the standard version of VIC without the addition of groundwater, please use the version directly from the VIC repository: https://github.com/UW-Hydro/VIC.
If you make use of this model, please acknowledge Scheidegger et al., 2021 (https://doi.org/10.3390/w13050663), and the VIC papers: Hamman et al., 2018 (DOI:10.5194/gmd-11-3481-2018) and Liang et al., 1994 (http://dx.doi.org/10.1029/94jd00483).
This version of VIC-AMBHAS-GRID can be used with a different resolution for the meteorological forcing data than the model files. For large models with a fine grid resolution, the forcing data can become very large. Since climate data are often at a lower resolution, e.g. 0.25 degrees, with the GRID version, the resolution of these can remain unchanged. For each model cell, the addition of the GRID module will find the coresponding cell in the forcing files.

## Preparation of input files

### VIC input files

The VIC model inputs consists of a global parameter file, meteorological forcing files, a parameter file and a domain file.  The global parameter file is the main input file and points to the location of the parameter, forcing and domain files, as well as setting model timings and output options. The meteorological forcing file, parameter file and domain file are spatially distributed files in netcdf format. With the GRID module, the forcing files can have a different spatial resolution than the model. Description, structure and format of each of them is described in the VIC image driver documentation (https://vic.readthedocs.io/en/master/Documentation/Drivers/Image/Inputs/). 
The input files for the coupled VIC-AMBHAS describing the land surface, model domain, and forcing are unaltered from the original VIC input files. However, since the baseflow formulation in VIC is replaced with the groundwater recharge calculation, the parameters Ds, Dsmax, Ws and c are not used in the model, but dummy values need to be set.

### AMBHAS input files

The input files for the groundwater model consist of:
- global parameter file

- groundwater data file

- initial hydraulic heads filed

- file describing the observation points

#### Global parameter file

The global parameter file specifies spatial and temporal model parameters related to the model grid and the time stepping and frequency of output to be written. 
In addition, the filenames of the groundwater data file, the initial heads are specified here. 
Model options, such as whether hydraulic conductivity or transmissivity are read in, whether groundwater abstraction takes place and if the specific yield is different in the soil and the aquifer are given.

The first string of the input file of each line specifies the parameter, followed by one or two parameters as follows:

0. Name of the file containing the spatially distributed groundwater data. E.g. gw_data.nc
1. Number of rows (NROW) in the groundwater parameter file. If the VIC model domain has no border of NA, then NROW should extend this on either side of the model domain. If the VIC domain contains one row and column of NA around the model domain, NROW in AMBHAS is equal to NROW in VIC.
2. Number of columns (NCOL) in the groundwater parameter file. If the VIC model domain has no border of NA, then NCOL should extend this on either side of the model domain. If the VIC domain contains one row and column of NA around the model domain, NCOL in AMBHAS is equal to NCOL in VIC.
3. Interval of NTIME when AMBHAS model output is written to file.
4. Cell size in y. If the model grid is in lat/lon the cell size in lat is given (DLAT), if it is a metric grid, then the cell size in m is given. The spacing for both needs to be regular.
5. Cell size in x. If the model grid is in lat/lon the cell size in lon is given (DLON), if it is a metric grid, then the cell size in m is given. The spacing for both needs to be regular.
6. Length of each model time step (DT) in unit of days. This needs to be the same as in VIC.
7. Output options for the hydraulic head field to the netcdf file h_n.nc, where n is the time step:

	0: hydraulic head field is written to out every time step	
	
	1: hydraulic head field is only written out for the last time step	
	
	n: hydraulic head field is written out every n time steps
	
8. Simulation Mode: 

  0: dynamic steady state using the forcing data of the first year and pumping of the first time step
  
  1: transient simulation 
  
  3: dynamic steady state using the forcing data of the first year and pumping of the first year
  
9. Initial conditions for hydraulic head.

	0: DEM - 10m. No initial head file needs to be specified
	
	1: file for initial hydraulic heads is read in specified as filename.nc	
	
10. Option if hydraulic conductivity or transmissivity are read in.

	0: K [m/day]	
	
	1: T [m2/day]
	
11. Confined or unconfined aquifer. Regardless of choice, the soil and the aquifer are in connection. This defines if the transmissivity is calculated using the K * aquifer thickness or K *(head - the base of the aquifer).

	0: unconfined
	
	1: confined	
	
12. Flag, if the model dimensions are in m or decimal degrees. If the grid is in lat/lon, then the cell dimensions and area for each cell need to be read in gw_data.nc.

	0: Regular grid using m	
	
	1: Regular grid using decimal degrees. The cell dimensions for each cell need to be read in gw_data.nc	
	
13. The pumping flag determines whether groundwater pumping time series are read in:

  0: no pumping time series is read in 
  
  1: pumping time series is read in 
  
14. Stress period for which one pumping file is valid. This has to be a multiplier of DT
15. Flag if the specific yield varies between the soil and the aquifer:

  0: one value of Sy across the entire profile read in gw_data.nc 
  
  1: Sy has one value in the soil and one in the aquifer, with a linear transition to twice the soil thickness. The values to be read in gw_data.nc. 
  
16. The maximum recharge from the soil to the aquifer is limited to a fraction of total available water in the layer the water table is interaction with for one time step. This improves stability within the model when the water table is close to the soil layer. Usually, values between 0.01 and 0.1 work.

17. GRID_FLAG to define if the climate forcing has a different resolution than the model files




Example file: gw_global_parameters.dat

GW_PAM_NAME gw_data_1.nc

NROW 100

NCOL 200

NTIME 240

DLAT 0.05

DLON 0.05

DT 0.125

Outoption 28800

SimulationMode 1

Restart 1 ../Folder/file_name.nc

K/Trans 0

CONFINED 0

GRID 1

PUMPING 0

PUMPING_STRESS_PERIOD 240

VARIABLE_SY 1

MAX_RECHARGE_FRACTION 0.01

GRID_FLAG 1


---Comment below here---

0. groundwater parameter file name
1. number of rows (lat) 
2. number of cols (rows) 
3. number of time steps output is written
4. lat grid spacing
5. lon grid spacing
6. time step (in days)
7. Output option for h field in out_data.nc 0: every time step, 1: last time step, int: every int time step 
8. SimulationMode 0: dynamic steady state using the forcing data of the first year, 1: transient simulation
9. Use restart file as initial conditions, 0: no, 1:yes. filename=h_ini.nc
10. read in 0: K [m/day] or 1: Trans [m2/day]
11. 1: confined or 0: unconfined aquifer
12. 0: regular grid using DLAT and DLON, 1: irregular grid reading in cell dimension and cell area for each cell
13. 0: no pumping time series is read in, 1: pumping time series is read in
14. Stress period for which one pumping is valid, has to be a multiple of DT
15. 1: Sy has one value in the soil and one in the aquifer. Depth of the soil and Sy_soil are read in in the gw_data.nc file
16. The maximum recharge from the soil to the aquifer is limited to a fraction of total available water in the layer the water table is interaction with for one time step. This improves stability within the model.
17.0 GRID_FLAG  0: the forcing file is same as model domain, 1: the forcing files have a different grid to the domain


#### Observation boreholes

Observation boreholes can be specified, for which the hydraulic head value for every NTIME time step will be recorded.

The first line gives the number of boreholes. 

The subsequent lines specify a borehole name and their latitude and longitude.

Example file: gw_observation.dat

num_obs_bh 2

BH1 6500.0 2000.0

BH2 8500.0 2000.0

---Comment below here---

1. number of observation boreholes
2. lat lon of observation boreholes 1
3. lat lon of observation boreholes 2


#### Spatial model data

The spatial model data is read in via the netcdf file gw_data.nc. The variables are written to file in the order listed below.

The data for each cell required is:
- Sy [-]: specific yield 
	
- T [m2/day]: transmissivity – a value must be given, even if K is specified
	
- K [m/day]: hydraulic conductivity – a value must be given, even if T is spcified
	
- mask: active cells of the model domain are set to 1
	
- dem [m]: digital elevation model 
	
- zbase [m]: base of the aquifer in m above datum 
	
- zriver [m]: elevation of the river elevation, e.g DEM or DEM - 5
	
- driver [m]: thickness of the river bed, e.g. 1m
	
- C_eff [1/day]: Conductance for effluent aquifer (river-> aquifer) 
	
- C_in [1/day]: Conductance for influent aquifer (aquifer -> river)
	
- C_leak_eff [1/day]: Conductance for leakage cells effluent 	
	
- C_leak_in [1/day]: Conductance for leakage cells influent 	
	
- headBC : Head[m] of specific head boundary cells, or -999 for non-specified head boundary nodes 
	
- river_area [m2]:  River area of cells where a river is present	
	
- quifer_map: zones of 1 are unconfined aquifer, and zones of 0 are confined aquifer
	
if the model dimensions is in lat/lon, then the following needs to be specified:
- dist_c_n [m]: distance of the cell center to the cell center of the cell to the north 
	
- dist_c_e [m]: distance of the cell center to the cell center of the cell to the east 
	
- edge_n[m]: length of the northern edge of the cell 	
	
- edge_e [m]: length of the eastern edge of the cell 	
	
- cell_area [m2]: cell area of each cell in 

for each cell:		
- z_soil  [m]: total soil depth used in the VIC model	
	
- Sy_soil: specific yield of the soil as used in the VIC model[-]
	
	
### GW pumping - optional

One file every PUMPING_STRESS_PERIOD is read in. 

The filename is gw_pumping_YEAR_MONTH.nc, for which the date is specified in integers. 

The stress period read in through the global parameter file determines how often GW pumping is read in.

The data for each cell and time step required is:

- Pumping [m3/day]


## Output files


### Observation data

The file gw_observation_ts.out record the hydraulic head for each observation borehole and time step. The coordinates (lat/lon) of each borehole are given in the header.

### Hydraulic head field

The hydraulic head field for the time steps specified in the global parameter field is written to a file for each NTIME step.

## Folder structure

It is important that the folder structure relative to the folder from which VIC is run is maintained.

-	Folder from which VIC is run. Contains the vic_job.sh and the global parameter file of VIC. The VIC global parameter file of VIC specifies the paths to the VIC input data and VIC output data.
-	../AMBHAS: contains global parameter file, gw_data, hi_ini, gw_observation.dat, and gw_pumping_DAY_MONTH_YEAR_SECONDS.nc
-	../AMBHAS_OUT: AMBHAS head files and observational data are written into here


### GRID input files

The input files for the grid module consist of:
- grid global parameter file

- Forcing cell numbers

- VIC cell numbers

### Grid global parameter file
This file specifies the filenames of the VIC cell dimensions and the FORCING cell dimensions, the number of rows and columns of each, the cell size, the x origin and y origin.

The file is structured as follows, line by line:

1. file name of VIC cells
2. file name of Forcing cells
3. Number of rows of VIC cells
4. Number of cols of VIC cells
5. cell size in lat of VIC cells
6. cell size in lon of VIC cells
7. x origin of VIC cells
8. y origin of VIC cells
9. Number of rows of FORCE cells
10. Number of cols of FORCE cells
11. cell size in lat of FORCE cells
12. cell size in lon of FORCE cells
13. x origin of FORCE cells
14. y origin of FORCE cells

example file grid_global_params.dat
VIC_DOMAIN_NAME VIC_CellNoIndus_2kmPlain.nc
FORCE_DOMAIN_NAME Force_CellIndus_2kmPlain.nc
FORCE_DATA Force_CellIndus_2kmPlain.nc
NROW_VIC 581
NCOL_VIC  638
DLAT_VIC  0.01666667
DLON_VIC  0.01666667
x_origin_VIC 67.04167 
y_origin_VIC 23.475 
NROW_FORCE 57
NCOL_FORCE 81
DLAT_FORCE 0.25
DLON_FORCE 0.25
x_origin_FORCE 63.875
y_origin_FORCE 22.875

---Comment below here---
1. file name of VIC cells
2. file name of Forcing cells
3. Number of rows of VIC cells
4. Number of cols of VIC cells
5. cell size in lat of VIC cells
6. cell size in lon of VIC cells
7. x origin of VIC cells
8. y origin of VIC cells
9. Number of rows of FORCE cells
10. Number of cols of FORCE cells
11. cell size in lat of FORCE cells
12. cell size in lon of FORCE cells
13. x origin of FORCE cells
14. y origin of FORCE cells

### Forcing cell numbers
This is a netcdf file that numbers each cell of the forcing grid from 1:ncellForce, where ncellForce is the number of grid cells in the climate forcing data. The dimensions are "Lat" and "Lon".
The data for each cell required is:
- cell [-]: numbering from 1:ncellForce 

### VIC cell numbers
This is a netcdf file that maps the forcing cell number onto the VIC grid. The forcing cells are resampled onto the VIC grid using the nearest neighbour method. The dimensions are "Lat" and "Lon".
The data for each cell required is:
- cell [-]: numbering from 1:ncellForce 
