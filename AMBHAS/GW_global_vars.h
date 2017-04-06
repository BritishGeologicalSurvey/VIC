	
#include "vic_structs.h"

	/******************************************************************************
	* structure to store global variables for GW model
	* to be read in from gw_global_parameters.dat
	* 
	*****************************************************************************/
	typedef struct {	
	int NROW;
	int NCOL;
	int NTIME;
	float DLAT;
	float DLON;
	int DT;	
	int OUT_OPTION;//output option for out_data.nc 0: every time step, 1: last time step, int: every int time step
	int SIM_MODE;//simulation mode 0: steady state, 1: transient with initial conditions from input, 2: transient and initial 			conditions from steady state, 3: dynamic steady state, 4: transient with dynamic steady state as initial condtions
	int PERIOD;//period for dynamic steady state calculation
	float GLOBAL_ERROR;//global balance to be < GLOBAL_ERROR to fulfill steady state criteria in m3
	int RESTART; //Use restart file as initial conditions, 0: no, 1:yes. filename=h_ini.nc
	int NUMOBSBH;// number of observation boreholes
	int KorTRANS;// flag wether to read in K or Trans. 0: K, 1:Trans
	int CONFINED;// 0: unconfined 1: confined aquifer
	int GRID;//0: regular grid using DLAT and DLON, 1: irregular grid reading in cell dimension and cell area for each cell
	float *p_obslat;//pointer to array of lats of observation borehole
	float *p_obslon; //pointer to array of lons of observation borehole
	int *p_obslat_count;//pointer to array of counts of lons of observation borehole
	int *p_obslon_count;//pointer to array of counts of lats of observation borehole
	} gw_global_data_struct;





	/******************************************************************************
	 * This structure stores data for one grid point
	 *  
	 *****************************************************************************/
	typedef struct {
	int NROW;//number of rows
	int NCOL;//number of cols
	double *lattitude;
	double *longitude;
	double **Sy;
	double **Trans;
	double **K;
	double **mask; //mask is read in from the forcing.nc file
	double **dem;
	double ** C_eff; //conductance for river effluent (aquifer to river)
	double **C_in; //conductance for river influent (river to aquifer)
	double **C_leak_eff; //conductance for leakage effluent
	double **C_leak_in; //conductance for leakage influent
	double **headBC; //flag whether cell is a specific head
	double **recharge;//m/day
	double **pumping;//m3/day
	double **river_area;//m2
	double **zbase; //base of aquifer
	double **c_n;//length from cell center to cell center to the north
	double **c_e;// length from cell center to cell center to the east
	double **e_n;//length of northern edge of the cell
	double **e_e;//length of eastern edge of the cell
	double **area;//cell area m2
	//VIC parameter
	int **V_ncell;//count of active grid cell on VIC domain

	} gw_data_struct;

	/******************************************************************************
	 * This structure stores gridded groundwater parameters and model output
	 *  
	 *****************************************************************************/
	typedef struct {
	double **Trans;//to be used in the simulated and updated once per time sep if unconfined mode
	double **h; //m
	double **hini; //m h at the beginning of the time step
	double **baseflow; //m3 per iteration
	double **baseflowTotal; //m3 per time step
	double **error; //balance of each cell m3
	double **hn; //m h of cell to the north
	double **hs; //m h of cell to the south
	double **he; //m h of cell to the east
	double **hw; //m h of cell to the west
	double **qn; //m q of cell to the north
	double **qs; //m q of cell to the south
	double **qe; //m q of cell to the east
	double **qw; //m q of cell to the west


	} gw_param_struct;

	/******************************************************************************
	 * This structure stores time series data for h for one grid point
	 *  
	 *****************************************************************************/
	typedef struct {
	double **h_obs;
	double *total_baseflow;
	double *balance;
	}gw_ts_struct;




	/******************************************************************************
	 * Function prototypes for the groundwater model
	 *  
	 *****************************************************************************/
	int GW_initialise(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts);
	int calculateGwInit(const gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p);
	float calculateGwFlow(const gw_global_data_struct *g, const gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, int n_time_step);//returns the global balance
	//calculate SS GW flow
	int calculateGwSS(const gw_global_data_struct *g, const gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts);
	int calculateDynamicGwSS(const gw_global_data_struct *g,  gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts);
	//int getGWData(gw_data_struct *p_gw_data );
	int GW_read_data(const gw_global_data_struct *g, gw_data_struct *d);
	int GW_read_h_ini(const gw_global_data_struct *g, gw_param_struct *p);
	int GW_read_Ts(const gw_global_data_struct *g, gw_data_struct *d, int ctime);
	int GW_write_output(const gw_global_data_struct *g, const gw_data_struct *d, const gw_param_struct *p, int ctime);
	int readGlobalData(gw_global_data_struct *g, gw_data_struct *d);
	int getObsDataCount(gw_global_data_struct *g, const gw_data_struct *d);
	int writeObsBH(const gw_global_data_struct *g, const gw_ts_struct *ts, int start, int nts, int flag);//flag 1: write, 2: append by one ts (set start to 0 and nts to nts+1, 3: append by nts (set start to start)
	int updateTrans(const gw_data_struct * d, gw_param_struct *p);
	double ** allocate2DArray( int nrow, int ncol);
	void free2DArray(double ** a, int nrow);
	void allocate_structs(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts);
	void free_structs(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts);


	/******************************************************************************
	 * Function prototypes for linking VIC and AMBHAS
	 *  
	 *****************************************************************************/

	int link_AMBHAS_VIC_Domain(gw_data_struct *g, domain_struct *vic_domain);
	int get_AMBHAS_Data_Into_VIC(const gw_data_struct *g,  const gw_param_struct *p, domain_struct *vic_domain, size_t time_step);
	int get_AMBHAS_Output_Into_VIC(gw_data_struct *g, domain_struct *vic_domain);
	int get_VIC_Data_Into_AMBHAS(gw_data_struct *g, domain_struct *vic_domain);

