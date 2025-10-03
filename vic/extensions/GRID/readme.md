GRID file description

This version of VIC-AMBHAS-GRID can be used with a different resolution for the meteorological forcing data than the model files. For large models with a fine grid resolution, the forcing data can become very large. Since climate data are often at a lower resolution, e.g. 0.25 degrees, with the GRID version, the resolution of these can remain unchanged. For each model cell, the addition of the GRID module will find the coresponding cell in the forcing files.


## GRID input files

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