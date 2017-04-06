
#include "GW_global_vars.h"
#include <stdlib.h>
void allocate_structs(gw_global_data_struct *g, gw_data_struct *d, gw_param_struct *p, gw_ts_struct *ts){

	//allocate arrays of gw_data_struct
	d->Sy=allocate2DArray(g->NROW, g->NCOL);
	d->Trans=allocate2DArray(g->NROW, g->NCOL);
	d->K=allocate2DArray(g->NROW, g->NCOL);
	d->mask=allocate2DArray(g->NROW, g->NCOL);
	d->dem=allocate2DArray(g->NROW, g->NCOL);
	d->C_eff=allocate2DArray(g->NROW, g->NCOL);
	d->C_in=allocate2DArray(g->NROW, g->NCOL);
	d->C_leak_eff=allocate2DArray(g->NROW, g->NCOL);
	d->C_leak_in=allocate2DArray(g->NROW, g->NCOL);
	d->headBC=allocate2DArray(g->NROW, g->NCOL);
	d->recharge=allocate2DArray(g->NROW, g->NCOL);
	d->pumping=allocate2DArray(g->NROW, g->NCOL);
	d->river_area=allocate2DArray(g->NROW, g->NCOL);
	d->zbase=allocate2DArray(g->NROW, g->NCOL);
	d->c_n=allocate2DArray(g->NROW, g->NCOL);
	d->c_e=allocate2DArray(g->NROW, g->NCOL);
	d->e_n=allocate2DArray(g->NROW, g->NCOL);
	d->e_e=allocate2DArray(g->NROW, g->NCOL);
	d->area=allocate2DArray(g->NROW, g->NCOL);

	d->lattitude=(double*) malloc(g->NROW * sizeof(double));
	d->longitude=(double*) malloc(g->NCOL * sizeof(double));


	//allocate arrays of gw_param_struct
	p->Trans=allocate2DArray(g->NROW, g->NCOL);
	p->h=allocate2DArray(g->NROW, g->NCOL);
	p->hini=allocate2DArray(g->NROW, g->NCOL);
	p->baseflow=allocate2DArray(g->NROW, g->NCOL);
	p->baseflowTotal=allocate2DArray(g->NROW, g->NCOL);
	p->error=allocate2DArray(g->NROW, g->NCOL);
	p->hn=allocate2DArray(g->NROW, g->NCOL);
	p->hs=allocate2DArray(g->NROW, g->NCOL);
	p->he=allocate2DArray(g->NROW, g->NCOL);
	p->hw=allocate2DArray(g->NROW, g->NCOL);
	p->qn=allocate2DArray(g->NROW, g->NCOL);
	p->qs=allocate2DArray(g->NROW, g->NCOL);
	p->qe=allocate2DArray(g->NROW, g->NCOL);
	p->qw=allocate2DArray(g->NROW, g->NCOL);

	//allocate arrays of gw_ts_struct
	ts->h_obs=allocate2DArray(g->NUMOBSBH, g->NTIME);
	ts->total_baseflow=(double*)malloc(g->NTIME*sizeof(double));
	ts->balance=(double*)malloc(g->NTIME*sizeof(double));



}
