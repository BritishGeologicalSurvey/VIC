# VIC_sample_data
Sample datasets for the Variable Infiltration Capacity (VIC) model including groundwater.
The model runs only with the image driver.
The VIC domain, forcing and parameter data are unchanged from the VIC example data set.

### Image Driver

**1. Stehekin**
- Domain: `image/Stehekin/parameters/domain.stehekin.20151028.nc`
- 10day Forcings: `image/Stehekin/forcings/Stehekin_image_test.forcings_10days.1949.nc`
- Global Parameters: `image/Stehekin/parameters/Stehekin_image_test.global.txt`
- Parameters: `image/Stehekin/parameters/Stehekin_test_params_20160327.nc`
    - `image/Stehekin/parameters/Stehekin_test_params_20160327.FROZEN_SOIL.nc` for the version where parameter `fs_active=1` in order to run option `FROZEN_SOIL=TRUE`

For the groundwater part, the following modifications are made:
-AMBHAS folder:
	-groundwater parameter file
	-global groundwater parameter file with filename: gw_global_parameters.data
	-observation file: gw_observation.dat
	for the structure of each file, please read the AMBHAS user guide.
-AMBHAS_OUT folder into which output is written.	
	