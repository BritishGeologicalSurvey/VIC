	

	#include "GW_global_vars.h"
	//#include <vic_run.h>
	#include <vic_driver_image.h>
	
	/******************************************************************************
	 * Function prototypes for linking VIC and AMBHAS
	 *  
	 *****************************************************************************/

	int link_AMBHAS_VIC_Domain(gw_data_struct *d, domain_struct *vic_domain);
	int get_AMBHAS_Data_Into_VIC(const gw_data_struct *g,  const gw_param_struct *p, domain_struct *vic_domain, size_t time_step);
	int get_AMBHAS_Output_Into_VIC(const gw_data_struct *d, const gw_param_struct *p, domain_struct *vic_domain);
	int get_VIC_Data_Into_AMBHAS(gw_data_struct *d, domain_struct *vic_domain);


	int GW_initialise(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, domain_struct *vic_domain);
	//calculate SS GW flow
	int calculateGwSS(const gw_global_data_struct *g, const gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, domain_struct *vic_domain);
	int calculateDynamicGwSS(const gw_global_data_struct *g,  gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts, domain_struct *vic_domain);
