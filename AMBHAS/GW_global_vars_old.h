
	//#include "vic_driver_shared_image.h"
	#include "vic_structs.h"//just for testing
	/******************************************************************************
	* global variables for GW model
	* to be read in from gw_global_parameters.dat
	* 
	*****************************************************************************/
	int NROW;
	int NCOL;
	int NTIME;
	int DLAT;
	int DLON;
	int DT;	
	int OUT_OPTION;//output option for out_data.nc 0: every time step, 1: last time step, int: every int time step
	int SIM_MODE;//simulation mode 0: steady state, 1: transient with initial conditions from input, 2: transient and initial 			conditions from steady state, 3: dynamic steady state, 4: transient with dynamic steady state as initial condtions
	int PERIOD;//period for dynamic steady state calculation
	int RESTART; //Use restart file as initial conditions, 0: no, 1:yes. filename=h_ini.nc
	int NUMOBSBH;// number of observation boreholes
	int KorTRANS;// flag wether to read in K or Trans. 0: K, 1:Trans
	int CONFINED;// 0: unconfined 1: confined aquifer
	float *p_obslat;//pointer of lats of observation borehole
	float *p_obslon; //pointer of lons of observation borehole
	int *p_obslat_count;//pointer of counts of lons of observation borehole
	int *p_obslon_count;//pointer of counts of lats of observation borehole



	/******************************************************************************
	 * This structure stores groundwater parameters and data for one grid point
	 *  
	 *****************************************************************************/
	typedef struct {
	double Sy;
	double Trans;
	double K;
	double watershedMask; //mask is read in from the forcing.nc file
	double dem;
	double h;//m
	double C_eff; //conductance for river effluent (aquifer to river)
	double C_in; //conductance for river influent (river to aquifer)
	double C_leak_eff; //conductance for leakage effluent
	double C_leak_in; //conductance for leakage influent
	double headBC; //flag whether cell is a specific head
	double lattitude;
	double longitude;
	double recharge;//m/day
	double pumping;//m3/day
	double balance;//m3/day
	double river_area;//m2
	double baseflow; //m3
	double V_ncell;//count of active grid cell on VIC domain
	double zbase; //base of aquifer
	} gw_data_struct;

	/******************************************************************************
	 * This structure stores time series data for h for one grid point
	 *  
	 *****************************************************************************/
	typedef struct {
	float h_obs;
	}gw_ts_struct;

	/******************************************************************************
	 * Function prototypes for the groundwater model
	 *  
	 *****************************************************************************/
	int GW_initialise(gw_data_struct *p_gw_data, gw_ts_struct *p_gw_ts);
	int calculateGwInit(gw_data_struct *p_gw_data);
	float calculateGwFlow(gw_data_struct *p_gw_data, gw_ts_struct *p_gw_ts, int n_time_step );//returns the global balance
	int calculateGwSS(gw_data_struct *p_gw_data, gw_ts_struct *p_gw_ts);//calculate SS GW flow
	int calculateDynamicGwSS(gw_data_struct *p_gw_data,  gw_ts_struct *p_gw_ts, int period);
	int getGWData(gw_data_struct *p_gw_data );
	int GW_read_data(gw_data_struct *p_gw_data);
	int GW_read_h_ini(gw_data_struct *p_gw_data);
	int GW_read_Ts(gw_data_struct *p_gw_data, int ctime);
	int GW_write_output(gw_data_struct *p_gw_data, int ctime);
	int readGlobalData();
	int getObsDataCount(gw_data_struct *p_gw_data);
	int writeObsBH(gw_ts_struct *p_gw_ts, int start, int nts, int flag);//flag 1: write, 2: append by one ts (set start to 0 and nts to nts+1, 3: append by nts (set start to start)
	int updateTrans(gw_data_struct *p_gw_data);
	int link_AMBHAS_VIC_Domain(gw_data_struct *p_gw_data, domain_struct *vic_domain);
	int get_AMBHAS_Data_Into_VIC(gw_data_struct *p_gw_data, domain_struct *vic_domain, size_t time_step);
	int get_AMBHAS_Output_Into_VIC(gw_data_struct *p_gw_data, domain_struct *vic_domain);
	int get_VIC_Data_Into_AMBHAS(gw_data_struct *p_gw_data, domain_struct *vic_domain);

