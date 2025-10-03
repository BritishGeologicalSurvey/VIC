	/******************************************************************************
	* structure to store grid variables 
	* to be read in from grid_global_parameters.dat
	* 
	*****************************************************************************/
	typedef struct {	
	int NROW_VIC; 
	int NCOL_VIC;
	float DLAT_VIC;
	float DLON_VIC;
 	float x_origin_VIC;
 	float y_origin_VIC;
 	double *lat_VIC; // lats of VIC grid
	double *lon_VIC; //lons of VIC grid
  double *lat_VIC_Force; //matching counter for each FORCE lat
  double *lon_VIC_Force; //matching counter for each FORCE lon
	double **cell_no_VIC; //NROW_VIC * NCOL_VIC
 	double **prec_VIC; //NROW_VIC * NCOL_VIC
	char VIC_name[100]; //Filename of groundwater parameter file
 
 	int NROW_FORCE;
	int NCOL_FORCE;
	float DLAT_FORCE;
	float DLON_FORCE;
	float x_origin_FORCE;
 	float y_origin_FORCE;
 	double *lat_FORCE; //lats of FORCE grid
	double *lon_FORCE; //lons of FORCE grid
	double **cell_no_FORCE; //NROW_FORCE * NCOL_FORCE
 	double **prec_FORCE; //NROW_FORCE * NCOL_FORCE
	char FORCE_name[100]; //Filename of groundwater parameter file	
 	char FORCE_Data_name[100]; //Filename of groundwater parameter file	
	} grid_data_struct;




	/******************************************************************************
	 * Function prototypes for the groundwater model
	 *  
	 *****************************************************************************/  
 	int readGridData(grid_data_struct *grid);
 	int grid_read_FORCE_Domain(grid_data_struct *grid);
 	int grid_read_FORCE_Data(grid_data_struct *grid);
 	int grid_read_VIC_Domain(grid_data_struct *grid);
 	

