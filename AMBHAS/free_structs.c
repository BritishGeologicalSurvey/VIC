#include "GW_global_vars.h"
#include <stdlib.h>

void free_structs(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts){

	//free arrays of gw_data_struct
	free2DArray((d->Sy),g->NROW);
	free2DArray((d->Trans),g->NROW);
	free2DArray((d->K),g->NROW);
	free2DArray((d->mask),g->NROW);
	free2DArray((d->dem),g->NROW);
	free2DArray((d->C_eff),g->NROW);
	free2DArray((d->C_in),g->NROW);
	free2DArray((d->C_leak_eff),g->NROW);
	free2DArray((d->C_leak_in),g->NROW);
	free2DArray((d->headBC),g->NROW);
	free2DArray((d->recharge),g->NROW);
	free2DArray((d->pumping),g->NROW);
	free2DArray((d->river_area),g->NROW);
	free2DArray((d->zbase),g->NROW);
	free2DArray((d->c_n),g->NROW);
	free2DArray((d->c_e),g->NROW);
	free2DArray((d->e_n),g->NROW);
	free2DArray((d->e_e),g->NROW);
	free2DArray((d->area),g->NROW);

	free(d->lattitude);
	free(d->longitude);


	//free arrays of gw_param_struct
	free2DArray((p->Trans),g->NROW);
	free2DArray((p->h),g->NROW);
	free2DArray((p->hini),g->NROW);
	free2DArray((p->baseflow),g->NROW);
	free2DArray((p->baseflowTotal),g->NROW);
	free2DArray((p->error),g->NROW);
	free2DArray((p->hn),g->NROW);
	free2DArray((p->hs),g->NROW);
	free2DArray((p->he),g->NROW);
	free2DArray((p->hw),g->NROW);
	free2DArray((p->qn),g->NROW);
	free2DArray((p->qs),g->NROW);
	free2DArray((p->qe),g->NROW);
	free2DArray((p->qw),g->NROW);

	//free  arrays of gw_ts_struct
	free2DArray((ts->h_obs),g->NUMOBSBH);

	free(ts->total_baseflow);
	free(ts->balance);


}
