/******************************************************************************
 * @section DESCRIPTION
 *
 * Stand-alone image mode driver of the VIC model, modified to include the AMBHAS groundwater model
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

/************************ AMBHAS code ************************/
#include "Link_AMBHAS_VIC.h"
/************************ end AMBHAS code ************************/

#include <vic_driver_image.h>
#include <rout.h>   // Routing routine (extension)


size_t              NF, NR;
size_t              current;
size_t             *filter_active_cells = NULL;
size_t             *mpi_map_mapping_array = NULL;
all_vars_struct    *all_vars = NULL;
force_data_struct  *force = NULL;
dmy_struct         *dmy = NULL;
dmy_struct          dmy_state;
filenames_struct    filenames;
filep_struct        filep;
domain_struct       global_domain;
global_param_struct global_param;
lake_con_struct    *lake_con = NULL;
domain_struct       local_domain;
MPI_Comm            MPI_COMM_VIC = MPI_COMM_WORLD;
MPI_Datatype        mpi_global_struct_type;
MPI_Datatype        mpi_filenames_struct_type;
MPI_Datatype        mpi_location_struct_type;
MPI_Datatype        mpi_alarm_struct_type;
MPI_Datatype        mpi_option_struct_type;
MPI_Datatype        mpi_param_struct_type;
int                *mpi_map_local_array_sizes = NULL;
int                *mpi_map_global_array_offsets = NULL;
int                 mpi_rank;
int                 mpi_size;
option_struct       options;
parameters_struct   param;
param_set_struct    param_set;
soil_con_struct    *soil_con = NULL;
veg_con_map_struct *veg_con_map = NULL;
veg_con_struct    **veg_con = NULL;
veg_hist_struct   **veg_hist = NULL;
veg_lib_struct    **veg_lib = NULL;
metadata_struct     state_metadata[N_STATE_VARS + N_STATE_VARS_EXT];
metadata_struct     out_metadata[N_OUTVAR_TYPES];
save_data_struct   *save_data;  // [ncells]
double           ***out_data = NULL;  // [ncells, nvars, nelem]
stream_struct      *output_streams = NULL;  // [nstreams]
nc_file_struct     *nc_hist_files = NULL;  // [nstreams]
double *temp_r; // [local_domain.ncells_active];
double *temp_gw; // [local_domain.ncells_active];
double *temp_baseflow; //[local_domain.ncells_active];
double *temp_z; //[local_domain.ncells_active];

// Extensions
rout_struct         rout; // Routing routine (extension)

double *out_recharge; //[global_domain.ncells_active];
double *out_gw_inflow; //[global_domain.ncells_active];
double *global_out_recharge; //[global_domain.ncells_total];
double *global_out_gw_inflow; //[global_domain.ncells_total];
double *cont_out_recharge; //[global_domain.ncells_total];
double *cont_out_gw_inflow; //[global_domain.ncells_total];
double *outp_baseflow; //[global_domain.ncells_total];
double *outp_z; //[global_domain.ncells_total];
double *outp_sy; //[global_domain.ncells_total;
int *local_order; //[local_domain.ncells_active];
int *ncells_run ; // [global_domain.ncells_total]
int *ncells_assign ; // [global_domain.ncells_total]
int *ncells_order ; // [global_domain.ncells_total]
int *g_mapping ; // [global_domain.ncells_total]
int * my_ncells ; // [local_domain.ncells_active]
int *total_ncells_active ; //[ntasks]
int tag1, tag2, tag3, tag4, tag5, tag6, tag7, dest, offset, sourc, count, j , proc, pos,cont, cts;
MPI_Status status_1;
MPI_Status status_2;
MPI_Status status_3;
MPI_Status status_4;
MPI_Status status_5;
MPI_Status status_6;
MPI_Status status_7;
int g_ncells_active, g_ncells_total; 
int RUNOFF_PRINT_FLAG=0; // flag wether to print water balance of a cell 
int RUNOFF_PRINT_CELL=0;//cell to write out water balance for each time step
float MAX_RECHARGE_FRACTION=0.0; //fraction of total available moisture in the soil layer the water table is in interaction with that can recharge the aquifer in one time step.
int SIM_MODE=0;
/******************************************************************************
 * @brief   Stand-alone image mode driver of the VIC model
 * @details The image mode driver runs VIC for a single timestep for all grid
 *          cells before moving on to the next timestep.
 *
 * @param argc Argument count
 * @param argv Argument vector
 *****************************************************************************/
int
main(int    argc,
     char **argv)
{

    printf("start vic_image \n");
    int          status;
    timer_struct global_timers[N_TIMERS];
    char         state_filename[MAXSTRING];
    int ntasks ;
    clock_t start_t, end_t; 
    double total_t;

	  start_t = clock();
  	printf("Starting of the program, start_t = %ld\n", start_t);

    // start vic all timer
    timer_start(&(global_timers[TIMER_VIC_ALL]));
    // start vic init timer
    timer_start(&(global_timers[TIMER_VIC_INIT]));

    // Initialize MPI - note: logging not yet initialized
    status = MPI_Init(&argc, &argv);
    if (status != MPI_SUCCESS) {
        fprintf(stderr, "MPI error in main(): %d\n", status);
        exit(EXIT_FAILURE);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    printf("after MPI initialize \n");
    // Initialize Log Destination
    initialize_log();
    printf("after initialize_log \n");
    // initialize mpi
    initialize_mpi();
    // process command line arguments
    if (mpi_rank == VIC_MPI_ROOT) {
        cmd_proc(argc, argv, filenames.global);
    }

    // read global parameters
    vic_image_start();
    printf("read global parameters \n");
    // allocate memory
    vic_alloc();
    printf("vic alloc \n");
    // allocate memory for routing
    rout_alloc();   // Routing routine (extension)

    // initialize model parameters from parameter files
    vic_image_init();
    printf("vic_image_init \n");
    // initialize routing parameters from parameter files
    rout_init();    // Routing routine (extension)

    // populate model state, either using a cold start or from a restart file
    vic_populate_model_state(&(dmy[0]));
    printf("vic_populate_model_state \n");
    // initialize output structures
    vic_init_output(&(dmy[0]));
    printf("vic_init_output \n");
    // Initialization is complete, print settings
    log_info(
        "Initialization is complete, print global param, parameters and options structures");
    print_global_param(&global_param);
    print_option(&options);
    print_parameters(&param);

    // stop init timer
    timer_stop(&(global_timers[TIMER_VIC_INIT]));
    // start vic run timer
    timer_start(&(global_timers[TIMER_VIC_RUN]));
    
    size_t                     i;

	end_t = clock();
	total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
	printf("VIC Initialised after: %f s\n", total_t);

    /************************ AMBHAS code ************************/
    printf("start AMBHAS code \n");  

  
  	int debug=0;
  	float balance;
  
  
  	//define structures containing global data
  	gw_global_data_struct global_data;
  	gw_global_data_struct *g;
  	g=&global_data;
  
  	gw_data_struct gw_data;
  	gw_data_struct *d;
  	d=&gw_data;
  
  	if (debug == 1){
  		printf("Created the global data structure for AMBHAS %d \n", mpi_rank);
  	}
  
  	//read the global data that tells us model dimensions and modes
  	readGlobalData(g, d);
  
  	if (debug == 1){
  		printf("Read in the global data for AMBHAS \n");
  	}
  
    //write  some global variables that have been read in from the AMBHAS global parameter file
    MAX_RECHARGE_FRACTION=g->MAX_RECHARGE_FRACTION;
    SIM_MODE=g->SIM_MODE;

  	//define structures and pointers to structures
  
  
  	gw_param_struct gw_param;
  	gw_param_struct *p;
  	p=&gw_param;
  
  	gw_ts_struct gw_ts;
  	gw_ts_struct *ts;
  	ts=&gw_ts;
  
  	if (debug == 1){
  		printf("Created the data structure for AMBHAS \n");
  	}	
  
  	allocate_structs(g, d, p, ts);
  
  	if (debug == 1){
  		printf("Allocated the structures for AMBHAS \n");
  	}
  
  
  	//read GW data from netcdf file
  	GW_read_data(g,d);
  	if (debug == 1){
  		printf("Gw read data has been called \n");
  	}
  
  
   //link AMBHAS grid with VIC local domains
    link_AMBHAS_Local_VIC_Domain(d, &local_domain, mpi_rank);
  	if (debug == 1){
  		printf("Link_AMBHAS_Local_VIC_Domain has been called \n");
  	}
  
    
    MPI_Barrier(MPI_COMM_WORLD);
    if (mpi_rank == VIC_MPI_ROOT) {
		//link AMBHAS grid with VIC global domain 
		if (debug == 1){
			printf(" check before Link_Ambhas_vic_domain on rank %d\n", mpi_rank);
		}
		link_AMBHAS_VIC_Domain(d, &global_domain);
		g_ncells_total=global_domain.ncells_total;
		g_ncells_active=global_domain.ncells_active;
		ncells_run = (int*) malloc(g_ncells_total*sizeof(int));
		total_ncells_active = (int*) malloc(ntasks*sizeof(int));
	  
		if (debug == 1){
			printf("link_AMBHAS_VIC has been called on mpi_rank %d  with %d active cells\n", mpi_rank, g_ncells_active);
		}
	}
  	MPI_Barrier(MPI_COMM_WORLD);
  	MPI_Bcast(&g_ncells_total,1, MPI_INT, VIC_MPI_ROOT, MPI_COMM_WORLD);
  	MPI_Bcast(&g_ncells_active,1, MPI_INT, VIC_MPI_ROOT, MPI_COMM_WORLD);
  	MPI_Barrier(MPI_COMM_WORLD);

	
        
	
    ncells_run = (int*) malloc(g_ncells_total*sizeof(int));

   	if (mpi_rank == VIC_MPI_ROOT){ 
      for(i =0 ; i < g_ncells_total ; i++ )
		    ncells_run[i]= global_domain.locations[i].run;
	}

  	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&ncells_run[0], g_ncells_total, MPI_INT, VIC_MPI_ROOT, MPI_COMM_WORLD);
  	MPI_Barrier(MPI_COMM_WORLD);
	
   	
  	i = local_domain.ncells_active ;
  	MPI_Barrier(MPI_COMM_WORLD);
  	MPI_Gather(&i, 1, MPI_INT, total_ncells_active,1, MPI_INT, VIC_MPI_ROOT, MPI_COMM_WORLD);
  	MPI_Barrier(MPI_COMM_WORLD);
	
  	ncells_assign = (int*) malloc(g_ncells_total*sizeof(int));
  	g_mapping = (int*) malloc(g_ncells_active*sizeof(int));
  	MPI_Barrier(MPI_COMM_WORLD);
  	debug=0;
   
  	if (mpi_rank == VIC_MPI_ROOT) {
		for (i=0 ; i < g_ncells_total ; i++ )
			ncells_assign[i]= -1;
	count = 0; 
	proc = 0; 
	for (i=0 ; i< g_ncells_total; i++ ){
		if ((count < ntasks) && (ncells_run[i]==1)){
				ncells_assign[i]= proc ;
				count ++ ;
				proc ++ ;
				}
				
		else{
			if ((count == ntasks) && (ncells_run[i]==1)){
				proc = 0 ;
				ncells_assign[i]= proc ;
				count = 1 ;
				proc ++ ;
			}
		}				
	}

	if (debug == 1){
		for (i=0 ; i< g_ncells_total; i++ )
			printf("ncells_assign: Rank: %d -- Ncell %d correspond to cell %d\n", mpi_rank, i, ncells_assign[i]);
	}
	
	count = 0;
	j = 0 ;
	for(i=0; i< g_ncells_total; i++ ){
		if (ncells_run[i] == 1)
		{
			g_mapping[j] = count;
			j++;
		}
		count ++ ;
	}

	if (debug == 1){   
		printf("After mapping on Root: %d -- g_ncells_total  %d -- local_domain.ncells_active %d --count %d j\n",  g_ncells_total,g_ncells_active, count, j);
		for (i=0 ; i< g_ncells_active; i++ )
			printf("g_mapping Rank: %d -- Ncell %d correspond to cell %d\n", mpi_rank, i, g_mapping[i]);	
			printf("Still running 1\n");		 
		}
  

	}//end root
   

    MPI_Barrier(MPI_COMM_WORLD);
    if (debug == 1){ 
		printf("Still running 2\n");
    }
    MPI_Bcast(&ncells_assign[0], g_ncells_total, MPI_INT, VIC_MPI_ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&g_mapping[0], g_ncells_active, MPI_INT, VIC_MPI_ROOT, MPI_COMM_WORLD);
    if (debug == 1){ 
		printf("Still running 3\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (debug == 1){ 
		printf("Still running 4\n");
		printf("My MPI Rank: %d -- g_ncells_total: %d local_domain.ncells_total,   %d local_domain.ncells_active %d\n", mpi_rank, g_ncells_total,local_domain.ncells_total, local_domain.ncells_active);
    }
 
  	my_ncells = (int*) malloc(local_domain.ncells_active*sizeof(int));
  	j = 0 ;
  	for(i =0; i < g_ncells_total ; i ++ )
  		if (ncells_assign[i] == mpi_rank ){
  			my_ncells[j]=i;
  			j ++ ;
  		}
  	if (debug == 1){	
        printf("Still running 5\n");
  		for (i=0 ; i< local_domain.ncells_active; i++ )
  			printf("my_ncells: Rank: %d -- Ncell %d correspond to cell %d\n", mpi_rank, i, my_ncells[i]);
  	}
   

	if (debug == 1){ 
		printf("Still running 6\n");
    }
      MPI_Barrier(MPI_COMM_WORLD);
      
    if (debug == 1){ 
		printf("Still running 7\n");
    }
  	//initialise h: read in GW_data, set initial conditions
  	GW_initialise(g, d, p, ts);
   
	if (debug == 1){ 
		printf("Still running 8\n");
	}
  
   	if (debug == 1){
  		printf("GW_initialise has been called \n");
  		printf("local domain ncells active %d ncells total %d \n", local_domain.ncells_active, g_ncells_total );
     
  	}

    temp_r = (double*) malloc(local_domain.ncells_active*sizeof(double));
    temp_gw = (double*) malloc(local_domain.ncells_active*sizeof(double));
    temp_baseflow = (double*) malloc(local_domain.ncells_active*sizeof(double));
    temp_z = (double*) malloc(local_domain.ncells_active*sizeof(double));
    local_order = (int*) malloc(local_domain.ncells_active*sizeof(int));    
    ncells_order = (int*) malloc(g_ncells_total*sizeof(int));   
    out_recharge = (double*) malloc(g_ncells_active*sizeof(double));
    out_gw_inflow = (double*) malloc(g_ncells_active*sizeof(double));
    global_out_recharge = (double*) malloc(g_ncells_total*sizeof(double));
    global_out_gw_inflow = (double*) malloc(g_ncells_total*sizeof(double));
    cont_out_recharge = (double*) malloc(g_ncells_total*sizeof(double));
    cont_out_gw_inflow = (double*) malloc(g_ncells_total*sizeof(double)); 
    outp_baseflow = (double*) malloc(g_ncells_total*sizeof(double));
    outp_z = (double*) malloc(g_ncells_total*sizeof(double));
    outp_sy = (double*) malloc(g_ncells_total*sizeof(double));


  	end_t = clock();
  	total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
   
  	printf("VIC_AMBHAS Initialised after: %f s\n", total_t);

    /************************ end AMBHAS code ************************/
  	cts=-1; //counter for time series	
	
    // loop over all timesteps
    for (current = 0; current < global_param.nrecs; current++) {
        // read forcing data
        timer_continue(&(global_timers[TIMER_VIC_FORCE]));
        vic_force();
			if (debug == 1){
				printf("vic_force has been called %d \n", mpi_rank);
			}
        timer_stop(&(global_timers[TIMER_VIC_FORCE]));

		printf("time step = %d \n", current);
		if(current==1){
			end_t = clock();
			total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
			printf("After time step 0: %f s\n", total_t);
		}
		cts++;

   // Write history files
        timer_continue(&(global_timers[TIMER_VIC_WRITE]));
        vic_write_output(&(dmy[current]));        timer_stop(&(global_timers[TIMER_VIC_WRITE]));

	/************************ AMBHAS code ****************************/
    //for all local domains:
		printf("before get_AMBHAS_Data_Into_VIC %d \n", mpi_rank, current);
		
		get_AMBHAS_Data_Into_VIC(d, p, &local_domain,(int)current);
		
        if (debug == 1){
	        printf("get_AMBHAS_Data_Into_VIC has been called for local domain on rank: %d time %d \n", mpi_rank, current);
	    }
                   
		if (mpi_rank == VIC_MPI_ROOT) {

			get_AMBHAS_Data_Into_Global_VIC(d, p, &global_domain,(int)current);
			
			if (debug == 1){
				printf("get_AMBHAS_Data_Into_VIC has been called for global domain on rank: %d time %d\n", mpi_rank, current);
			}

		}         
		/************************ end AMBHAS code ************************/
		



		// run vic over the domain
		vic_image_run(&(dmy[current]));
			if (debug == 1){
					printf("vic_image_run has been called %d \n", mpi_rank);
			}

		/************************ AMBHAS code ************************/

		 for (i =0; i < local_domain.ncells_active; i++){
			 temp_r[i] =  out_data[i][OUT_RECHARGE][0];
			 temp_gw[i] = out_data[i][OUT_GW_INFLOW][0];
			 cont = (mpi_rank + (ntasks * i));
			 local_order[i]= cont;
		}

		if (debug == 1){
			 for (i =0; i < local_domain.ncells_active; i++)
			 printf("Rank %d- position %d-relative_order %d\n", mpi_rank,i, local_order[i]); 

		}
		
		MPI_Barrier(MPI_COMM_WORLD); 

		if (mpi_rank != VIC_MPI_ROOT) {
			MPI_Send(&temp_r[0], local_domain.ncells_active, MPI_DOUBLE, VIC_MPI_ROOT, tag2, MPI_COMM_WORLD);
			MPI_Send(&temp_gw[0], local_domain.ncells_active, MPI_DOUBLE, VIC_MPI_ROOT, tag3, MPI_COMM_WORLD);
		}
		else{
			for (i =0; i < local_domain.ncells_active; i++ ){
				out_recharge[i]=temp_r[i];		
				out_gw_inflow[i]=temp_gw[i];
			}
			
			offset = local_domain.ncells_active ;
			for (sourc = 1; sourc< ntasks; sourc ++){
				MPI_Recv(&out_recharge[offset], total_ncells_active[sourc], MPI_DOUBLE, sourc, tag2, MPI_COMM_WORLD, &status_2);
				MPI_Recv(&out_gw_inflow[offset], total_ncells_active[sourc], MPI_DOUBLE, sourc, tag3, MPI_COMM_WORLD, &status_3);
				offset = offset + total_ncells_active[sourc];
			}

			pos=0;
			cont =0;
			for(i = 0 ; i <ntasks; i++){
				for (j=0; j < total_ncells_active[i]; j++){
					cont = (i + (ntasks * j));
					cont_out_recharge[cont]= out_recharge[pos];
					cont_out_gw_inflow[cont]= out_gw_inflow[pos];
					pos++;
				}
			}

			if (debug == 1){
				for(i=0; i< g_ncells_active; i++){
					printf("Check - recharge of %i is %f \n", i, cont_out_recharge[i]);
				}
			}
		
			for(i =0 ; i < g_ncells_active ; i ++ ) {
				if (debug == 1)
					printf("g_mapping %d is %d\n", i, g_mapping[i]);
				global_out_recharge[g_mapping[i]] = cont_out_recharge[i];
				global_out_gw_inflow[g_mapping[i]] = cont_out_gw_inflow[i];
			}
		
			if (debug == 1){
				for(i =0 ; i < g_ncells_total ; i ++ )
					printf("test  - global recharge of cell % d is %f\n", i, global_out_recharge[i]);
			} 
	   
			get_VIC_Data_Into_AMBHAS(d,g, &global_domain, global_out_recharge, global_out_gw_inflow);
			
			if (debug == 1){
				printf("get_VIC_Data_Into_AMBHAS has been called by process %d\n", mpi_rank);
			}

			if(g->SIM_MODE==1||g->SIM_MODE==2||g->SIM_MODE==4){
					
				//read in time series for pumping	
				if(g->PUMPING == 1){

					GW_read_1pumping(g, d, dmy[current].year,dmy[current].month, dmy[current].day,dmy[current].dayseconds);
					if (debug == 1){
						printf("GW_read_1pumping has been called \n");
					}
				}
      }// end if sim mode...
      
      //if steady state - use the first pumping time step
 			if(g->SIM_MODE==0){
					
				//read in time series for pumping	
				if(g->PUMPING == 1){

					GW_read_1pumping(g, d, dmy[0].year,dmy[0].month, dmy[0].day,dmy[0].dayseconds);
					if (debug == 1){
						printf("GW_read_1pumping has been called \n");
					}
				}
        
 			}// end if sim mode...
      
      //if dynamic steady state - use the first year of pumping data
 			if(g->SIM_MODE==3){
					
				//read in time series for pumping	
				if(g->PUMPING == 1){

					GW_read_1pumping(g, d, dmy[current].year,dmy[0].month, dmy[current].day,dmy[current].dayseconds);
					if (debug == 1){
						printf("GW_read_1pumping has been called \n");
					}
				}
        
 			}// end if sim mode...
		
			//calculate gw flow for one time step
			balance=calculateGwFlow(g,d,p,ts, cts);
			if (debug == 1){
					printf("calculateGwFlow has been called \n");
			}

			if(g->OUT_OPTION == 0){
								   
				//write out head at every time step
				GW_write_output(g,d,p,(int)current);
				if (debug == 1){
					printf("GW_write_output has been called \n");
				}
			}
			if(g->OUT_OPTION>1){              
				if((int)current % g->OUT_OPTION==0)
		
				//write out head at every OUT_OPTION time step
				{
					GW_write_output(g,d,p, (int)current);
					if (debug == 1){
						printf("GW_write_output has been called \n");
					}

				}
			}
		
			//create time series output file
			if((int)current ==0){
				writeObsBH(g,ts, 0, 1, 1);
				
			}	

	  
				//write time series data every g->NTIME time step
			if(((int)current+1) % g->NTIME==0){
				if((int)current>0){
					writeObsBH(g,ts, (int)current-g->NTIME, g->NTIME, 4);
					cts=-1;
				}
			}

			

			get_AMBHAS_Output_Into_VIC(d, p, &global_domain, outp_baseflow, outp_z, outp_sy);
				
			if (debug == 1){
				printf("Rank- %d - get_AMBHAS_Output_Into_VIC has been called \n", mpi_rank);
			}  
				 
		}//end on root



		MPI_Barrier(MPI_COMM_WORLD); 
		MPI_Bcast(&outp_baseflow[0],g_ncells_total, MPI_DOUBLE, VIC_MPI_ROOT, MPI_COMM_WORLD);
		MPI_Bcast(&outp_z[0],g_ncells_total, MPI_DOUBLE, VIC_MPI_ROOT, MPI_COMM_WORLD);
		MPI_Bcast(&outp_sy[0],g_ncells_total, MPI_DOUBLE, VIC_MPI_ROOT, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		cont =0;
		for (j = 0; j < g_ncells_total ; j ++ ){
			if (ncells_assign[j] == mpi_rank )
			{	
				out_data[cont][OUT_BASEFLOW_AQ][0] = outp_baseflow[j];
				out_data[cont][OUT_Z][0] = outp_z[j];
				out_data[cont][OUT_SY][0] = outp_sy[j];
				if (debug == 1){
					printf("My rank %d, post %d, cell %d  baseflow %f - z %f \n", mpi_rank,cont,j,out_data[cont][OUT_BASEFLOW_AQ][0], out_data[cont][OUT_Z][0]);
				}
			cont ++ ;
			}
		}

		for (i = 0; i < options.Noutstreams; i++) {
			agg_stream_data(&(output_streams[i]), &(dmy[current]), out_data);
		}
		 
		MPI_Barrier(MPI_COMM_WORLD); 
		/************************ end AMBHAS code ************************/
			  
			// Write history files
			vic_write_output(&(dmy[current]));
			if (debug == 1){
						printf("vic_write output has been called \n");
			}

			// Write state file
			if (check_save_state_flag(current)) {
				debug("writing state file for timestep %zu", current);
				vic_store(&(dmy[current]), state_filename);
				debug("finished storing state file: %s", state_filename)
			}
			if (debug == 1){
						printf("vic_write state file has been called \n");
			}


    }//end for each time step loop
    /************************ AMBHAS code ************************/
    //write out final head
	MPI_Barrier(MPI_COMM_WORLD); 
	if (mpi_rank == VIC_MPI_ROOT) {

		if(g->OUT_OPTION==1)
		{
			GW_write_output(g,d,p, current);
		}    }
    MPI_Barrier(MPI_COMM_WORLD); 

    /************************ end AMBHAS code ************************/
    // stop vic run timer
    timer_stop(&(global_timers[TIMER_VIC_RUN]));
    // start vic final timer
    timer_start(&(global_timers[TIMER_VIC_FINAL]));
    // clean up
    vic_image_finalize();

    // clean up routing
    rout_finalize();    // Routing routine (extension)

    // finalize MPI
    status = MPI_Finalize();
    if (status != MPI_SUCCESS) {
        log_err("MPI error: %d", status);
    }

    log_info("Completed running VIC %s", VIC_DRIVER);

    // stop vic final timer
    timer_stop(&(global_timers[TIMER_VIC_FINAL]));
    // stop vic all timer
    timer_stop(&(global_timers[TIMER_VIC_ALL]));

    if (mpi_rank == VIC_MPI_ROOT) {
        // write timing info
        write_vic_timing_table(global_timers, VIC_DRIVER);
    }
	/************************ AMBHAS code ************************/
	free(temp_r); // [local_domain.ncells_active];
	free(temp_gw); // [local_domain.ncells_active];
	free(temp_baseflow); //[local_domain.ncells_active];
	free(temp_z); //[local_domain.ncells_active];
	free(out_recharge); //[global_domain.ncells_active];
	free(out_gw_inflow); //[global_domain.ncells_active];
	free(outp_baseflow); //[global_domain.ncells_active];
	free(outp_z); //[global_domain.ncells_active];
	free(outp_sy); //[global_domain.ncells_active];
	free(total_ncells_active); //[ntasks] 
	free(ncells_assign);
	free(my_ncells);
	free(ncells_run); 
	free(g_mapping); 
	free(global_out_recharge); //[global_domain.ncells_active];
	free(global_out_gw_inflow); //[global_domain.ncells_active];
	free(cont_out_recharge); //[global_domain.ncells_active];
	free(cont_out_gw_inflow); //[global_domain.ncells_active];
	free(local_order); //[local_domain.ncells_active];
	free(ncells_order) ; // [global_domain.ncells_total]
	/************************ end AMBHAS code ************************/
    return EXIT_SUCCESS;
}


