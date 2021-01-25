/******************************************************************************
* @section DESCRIPTION
*
* Calculate infiltration and runoff from the surface, gravity driven drainage
* between all soil layers, and generates baseflow from the bottom layer.
* 
* This function has been modified by JS to calculate groundwater recharge using the SIMGM 
* model by Niu et al. 2007, replacing VIC's baseflow formulation.
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
******************************************************************************/

#include <vic_run.h>

/******************************************************************************
* @brief    Calculate infiltration and runoff from the surface, gravity driven
*           drainage between all soil layers, and groundwater recharge from the
*           bottom layer. The baseflow calculation is not used here and baseflow
*           is replaced by the baseflow calculation in AMBHAS.	
******************************************************************************/
int
runoff(cell_data_struct  *cell,
       energy_bal_struct *energy,
       soil_con_struct   *soil_con,
       double             ppt,
       double            *frost_fract,
       int                Nnodes,
       double 		  Sy,
       double 		  z)
{
	extern option_struct       options;
    extern global_param_struct global_param;
    int debug=0;
    size_t                     lindex;
    size_t			wtindex; //index of water table layer
    size_t                     time_step;
    int                        last_index;
    int                        tmplayer;
    int                        fidx;
    int                        ErrorFlag;
    double                     A, frac;
    double                     tmp_runoff;
    double                     inflow;
    double                     resid_moist[MAX_LAYERS]; // residual moisture (mm)
    double                     org_moist[MAX_LAYERS]; // total soil moisture (liquid and frozen) at beginning of this function (mm)
    double                     avail_liq[MAX_LAYERS][MAX_FROST_AREAS]; // liquid soil moisture available for evap/drainage (mm)
    double                     liq[MAX_LAYERS]; // current liquid soil moisture (mm)
    double                     ice[MAX_LAYERS]; // current frozen soil moisture (mm)
    double                     moist[MAX_LAYERS]; // current total soil moisture (liquid and frozen) (mm)
    double                     max_moist[MAX_LAYERS]; // maximum storable moisture (liquid and frozen) (mm)
    double                     Ksat[MAX_LAYERS];//mm/timestep
    double                     Q12[MAX_LAYERS - 1];
    double                     Q1[MAX_FROST_AREAS];//Q1 for output
    double                     Q2[MAX_FROST_AREAS];//Q2 for output
    double                     Dsmax;
    double                     tmp_inflow;
    double                     tmp_moist;
    double                     tmp_moist_for_runoff[MAX_LAYERS];
    double                     tmp_liq;
    double                     dt_inflow;
    double                     dt_runoff;
    double                     runoff[MAX_FROST_AREAS];
    double                     recharge[MAX_FROST_AREAS];	//groundwater recharge to be passed into AMBHAS
    double                     dt_recharge; //groundwater recharge to be passed into AMBHAS
    double                     temp_recharge;//recharge from layer above into groundwater
    double                     dt_gwinflow;//inflow from groundwater when water table is within the soil column
    double                     gw_inflow[MAX_FROST_AREAS];	//inflow from groundwater when water table is within the soil column. accounted for in VIC and AMBHAS!!
    double                     zwt;//water table depth used to calculate groundwater recharge zwt is in cm
    double                     tmp_dt_runoff[MAX_FROST_AREAS];
    double                     baseflow[MAX_FROST_AREAS];
    double                     dt_baseflow;
    double                     rel_moist;
    double                     evap[MAX_LAYERS][MAX_FROST_AREAS];
    double                     sum_liq;
    double                     evap_fraction;
    double                     evap_sum;
    double                     depth[MAX_LAYERS]; //in m
    double                     depth_sum[MAX_LAYERS]; //layer depth in m
    double                     node_depth[MAX_LAYERS]; //node depth of each layer in m
    double                     node_depth_wt; //node depth just above the water table in m
    double                     bubble[MAX_LAYERS];//bubbling pressure
    double                     psi_m;//matic potential
    double                     psi_sat;//matic potential for saturated soil
    double                     zbot; //depth of the bottom layer
    layer_data_struct         *layer;
    layer_data_struct          tmp_layer;
    unsigned short             runoff_steps_per_dt;
    int                        water_table_layer;//layer in which the water table occurs
    double                     kr[MAX_LAYERS];// relative permeability
    double liq_init[MAX_LAYERS];//initial liq distribution
    extern int RUNOFF_PRINT_FLAG;
    extern int RUNOFF_PRINT_CELL;
    extern float MAX_RECHARGE_FRACTION;
    extern int IRRIGATE_ABSTRACTED_WATER;
    

	//printf("max recharge fraction %f  \n", MAX_RECHARGE_FRACTION);
   
   //don't print anything 
    RUNOFF_PRINT_FLAG=0;

    /** Set Residual Moisture **/
    for (lindex = 0; lindex < options.Nlayer; lindex++) {
        resid_moist[lindex] = soil_con->resid_moist[lindex] *
                              soil_con->depth[lindex] * MM_PER_M;                     
    }
    
    /** Allocate and Set Values for Soil Sublayers **/
    layer = cell->layer;
    zwt = cell->zwt;

    cell->runoff = 0;
    cell->baseflow = 0;
    cell->asat = 0;
    cell->recharge = 0;
    cell->gw_inflow = 0;
    cell->Q1 = 0;
    cell->Q2 = 0;



    runoff_steps_per_dt = global_param.runoff_steps_per_day /
                          global_param.model_steps_per_day;

    for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
        baseflow[fidx] = 0;
        recharge[fidx] = 0;
        gw_inflow[fidx]=0;
        Q1[fidx]=0.0;
        Q2[fidx]=0.0;
    }

    for (lindex = 0; lindex < options.Nlayer; lindex++) {
        evap[lindex][0] = layer[lindex].evap / (double) runoff_steps_per_dt;
        org_moist[lindex] = layer[lindex].moist;       
        layer[lindex].moist = 0;
        if (evap[lindex][0] > 0) { // if there is positive evaporation
            sum_liq = 0;
            // compute available soil moisture for each frost sub area.
            for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
                avail_liq[lindex][fidx] =
                    (org_moist[lindex] - layer[lindex].ice[fidx] -
                     resid_moist[lindex]);
                if (avail_liq[lindex][fidx] < 0) {
                    avail_liq[lindex][fidx] = 0;
                }
                sum_liq += avail_liq[lindex][fidx] *
                           frost_fract[fidx];
            }
            // compute fraction of available soil moisture that is evaporated
            if (sum_liq > 0) {
                evap_fraction = evap[lindex][0] / sum_liq;
            }
            else {
                evap_fraction = 1.0;
            }
            // distribute evaporation between frost sub areas by percentage
            evap_sum = evap[lindex][0];
            for (fidx = (int)options.Nfrost - 1; fidx >= 0; fidx--) {
                evap[lindex][fidx] = avail_liq[lindex][fidx] * evap_fraction;
                avail_liq[lindex][fidx] -= evap[lindex][fidx];
                evap_sum -= evap[lindex][fidx] * frost_fract[fidx];
            }
        }
        else {
            for (fidx = (int)options.Nfrost - 1; fidx > 0; fidx--) {
                evap[lindex][fidx] = evap[lindex][0];
            }
        }
    }

    for (fidx = 0; fidx < (int)options.Nfrost; fidx++) {
        /** ppt = amount of liquid water coming to the surface **/

        inflow = ppt; //js: is the same as cell->inflow
        


        /**************************************************
           Initialize Variables
        **************************************************/
        water_table_layer=options.Nlayer+1;//initialise the water table below the soil layers (layer 4)	

        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            Ksat[lindex] = soil_con->Ksat[lindex] /
                           global_param.runoff_steps_per_day;

            /** Set Layer Liquid Moisture Content **/
            liq[lindex] = org_moist[lindex] - layer[lindex].ice[fidx];


            /** Set Layer Frozen Moisture Content **/
            ice[lindex] = layer[lindex].ice[fidx];

            /** Set Layer Maximum Moisture Content **/
            max_moist[lindex] = soil_con->max_moist[lindex];

	
            /********************************************************************************************
               Find the depth of each layer, and the layer in which the groundwater table z is located
		            ADDED FOR AMBHAS
            ********************************************************************************************/
            depth_sum[lindex]=0.0; //initialise layer depth to 0
            node_depth[lindex]=0.0;



            //set depth [m]
            depth[lindex]=soil_con->depth[lindex];

            //set total depth of each layer
            if(lindex==0){
		            depth_sum[lindex]=depth[lindex];
		            node_depth[lindex]=depth[lindex]/2.0;	
            }
            if(lindex>0){
		            depth_sum[lindex]=depth[lindex]+depth_sum[lindex-1];
		            node_depth[lindex]=depth_sum[lindex-1]+depth[lindex]/2.0;					
            }
                if(debug==1){
                  printf("depth_sum %f node depth %f for layer %d z %f \n", depth_sum[lindex], node_depth[lindex], lindex, z);
            }

            //find the soil layer in which the water table occurs
            if(z<=depth_sum[lindex]){ 
            		if(water_table_layer==options.Nlayer+1){ //only change if water_table_layer has its original value
            		    water_table_layer=lindex;	//set water_table_layer to the current index
		            }
            }

            //set bubbling pressure should be -ive
            bubble[lindex]=soil_con->bubble[lindex];
            if(bubble[lindex]>0){
                bubble[lindex]=-bubble[lindex];
            }
			if(debug==1){
				printf("bubbling pressure %f layer %d\n",bubble[lindex], lindex);
			}

        }//end for Nlayer
        
        zbot=depth_sum[options.Nlayer-1];	


        debug=0;
            if(debug==1){
				if(water_table_layer<4){
					printf("water table layer %d, depth_sum %f, zbot %f, z %f Sy %f \n",water_table_layer, depth_sum[options.Nlayer-1], zbot, z, Sy);
				}
			}
        debug=0;
        /********************************************************************************************
	    	END ADDED FOR AMBHAS
        ********************************************************************************************/

        /******************************************************
           Runoff Based on Soil Moisture Level of Upper Layers
        ******************************************************/

        for (lindex = 0; lindex < options.Nlayer; lindex++) {
            tmp_moist_for_runoff[lindex] = (liq[lindex] + ice[lindex]);
        }
        compute_runoff_and_asat(soil_con, tmp_moist_for_runoff, inflow, &A,
                                &(runoff[fidx]));

        // save dt_runoff based on initial runoff estimate,
        // since we will modify total runoff below for the case of completely saturated soil
        tmp_dt_runoff[fidx] = runoff[fidx] /
                              (double) runoff_steps_per_dt;

        /**************************************************
           Compute Flow Between Soil Layers ()
        **************************************************/

        dt_inflow = inflow / (double) runoff_steps_per_dt;

        Dsmax = soil_con->Dsmax / global_param.runoff_steps_per_day;

        for (time_step = 0; time_step < runoff_steps_per_dt; time_step++) {
        
            inflow = dt_inflow;
            
            //initialise gw parameters
            dt_gwinflow=0.0;
            dt_recharge=0.0;
            temp_recharge=0.0;

            /*************************************
               Compute Drainage between Sublayers
            *************************************/
            /**if the water level is below the soil column, use Q12 as represented in VIC **/

            for (lindex = 0; lindex < options.Nlayer - 1; lindex++) { //for layers 0 and 1
                /** Brooks & Corey relation for hydraulic conductivity **/
	              liq_init[lindex]=liq[lindex];//js added for balance calculation
		            if(debug==1){
	                  printf("liq_init %d %f \n", lindex, liq[lindex]);
		            }

                if ((tmp_liq = liq[lindex] - evap[lindex][fidx]) <
                    resid_moist[lindex]) {
                    tmp_liq = resid_moist[lindex];
                }
               kr[lindex]=pow(((tmp_liq -
                                        resid_moist[lindex]) /
                                       (soil_con->max_moist[lindex] -
                                        resid_moist[lindex])),
                                      soil_con->expt[lindex]);

          
                if (liq[lindex] > resid_moist[lindex]) {
                    Q12[lindex] = Ksat[lindex] * kr[lindex]; //in mm
                }
                else {
                    Q12[lindex] = 0.;
                }
				if(debug==1){
					printf("layer %d, kr %f, Q12 %f, liq %f max_moist %f \n", lindex, kr[lindex],  Q12[lindex], liq[lindex], max_moist[lindex]);   
				}
            }
 
            //set relative permeability for the bottom layer
            lindex=options.Nlayer - 1;

            liq_init[lindex]=liq[lindex];//js added for balance calculation
            if(debug==1){
            		printf("liq_init %d %f \n", lindex, liq[lindex]);
            }
            
            if ((tmp_liq = liq[lindex] ) <
                    resid_moist[lindex]) {
                    tmp_liq = resid_moist[lindex];
                }
                kr[lindex]=pow(((tmp_liq -
                                        resid_moist[lindex]) /
                                       (soil_con->max_moist[lindex] -
                                        resid_moist[lindex])),
                                      soil_con->expt[lindex]); 
		   if(debug==1){
					printf("layer %d, kr %f, liq %f, max_moist %f \n ", lindex, kr[lindex], liq[lindex], max_moist[lindex]); 
			}
	         

 		    //if the water table is above the depth of the bottom layer, use eq 9 from Niu et al. 2007  Added for AMBHAS
            if(z<=zbot){
                lindex=water_table_layer;
                node_depth_wt=node_depth[lindex-1];//should be lindex-1
                wtindex=lindex-1;//should be lindex-1
                if(lindex==0){//if the top layer, set the depth to 0 and the wtindex to 0
                node_depth_wt=0.0;
                wtindex=lindex;
            }
            
            if(debug==1){
                printf("lindex % d, node_depth, %f, wtindex %d, \n", lindex, node_depth_wt, wtindex);
            }
            
            
            	/** Calculate saturated matric potential for layer that is in the water table
             		 psi_m = psi_e(moist/max_moist)^(-b)  Campbell 1985 eq 5.9**/
            
           	psi_sat=bubble[lindex]/100.0; // in m  

            psi_m=bubble[wtindex] /100.0 * pow((liq[wtindex]/soil_con->max_moist[wtindex]),
            ((-soil_con->expt[wtindex]+3.0)/2.0));//in m

            //eq 9 from Niu et al 2007, Ksat is in mm/timestep
            //assume that K=Ksat at the water table
            
            dt_gwinflow=-(2.0/(1.0/( kr[lindex]*Ksat[lindex])+1.0/(kr[wtindex]* Ksat[wtindex]))) * 
               (((psi_sat-z)-(psi_m-node_depth_wt))/(z-node_depth_wt));
            if(debug==1){
                printf("dt_gwinflow %f \n", dt_gwinflow);
            }
            
            if(RUNOFF_PRINT_FLAG==1){
                printf("layer_number_of_water_table %d dt_gwinflow %f gradient %f psi_sat %f psi_m %f node_depth_wt %f z %f K %f Ksat %f",  wtindex, dt_gwinflow,  (((psi_sat-z)-(psi_m-node_depth_wt))/(z-node_depth_wt)),psi_sat, psi_m, node_depth_wt, z, (2.0/(1.0/( kr[lindex]*Ksat[lindex])+1.0/(kr[wtindex]* Ksat[wtindex]))),Ksat[wtindex]);
            }
            
            
            
            // limit dt_gwinflow to MAX_RECHARGE_FRACTION of liquid from the layer above if dt_gwinflow is positive
            if(dt_gwinflow>liq[wtindex]*(MAX_RECHARGE_FRACTION)){
				/*if(RUNOFF_PRINT_FLAG==1){
				printf("max_moist_of_layer_%f_moist_of_layer_%f_layer_number_of_water_table_%d_dt_gwinflow_%f_gradient_%f_K_%f_Ksat_%f", soil_con->max_moist[wtindex] , liq[wtindex], wtindex, dt_gwinflow,  (((psi_sat-z)-(psi_m-node_depth_wt))/(z-node_depth_wt)),(2.0/(1.0/( kr[lindex]*Ksat[lindex])+1.0/(kr[wtindex]* Ksat[wtindex]))),Ksat[wtindex]);
				}*/
            
            dt_gwinflow=liq[wtindex]*(MAX_RECHARGE_FRACTION);
            
            }
            //limit dt_gwinflow toMAX_RECHARGE_FRACTION of liquid from the layer above if dt_gwinflow is negative
            if(dt_gwinflow<-liq[wtindex]*(MAX_RECHARGE_FRACTION)){
				/*if(RUNOFF_PRINT_FLAG==1){
				printf("max_moist_of_layer_%f_moist_of_layer_%f_layer_number_of_water_table_%d_dt_gwinflow_%f_gradient_%f_K_%f_Ksat_%f", soil_con->max_moist[wtindex] , liq[wtindex], wtindex, dt_gwinflow,  (((psi_sat-z)-(psi_m-node_depth_wt))/(z-node_depth_wt)),(2.0/(1.0/( kr[lindex]*Ksat[lindex])+1.0/(kr[wtindex]* Ksat[wtindex]))),Ksat[wtindex]);
				}*/
                dt_gwinflow=-liq[wtindex]*(MAX_RECHARGE_FRACTION);
            
            }
            
            //if water table is at or higher than the top layer, set dt_gwinflow to Sy*(z-depth[lindex])*1000;
            if(lindex==0){
                dt_gwinflow=Sy*(z-depth[lindex])*1000;         
				//RUNOFF_PRINT_FLAG=1;
            }

            //dt_gwinflow is positive, remove moisture from layer above and add to gw recharge, set Q12 to 0
            if(dt_gwinflow>0.0){
            liq[wtindex]=liq[wtindex]-dt_gwinflow;
            temp_recharge=dt_gwinflow;
            dt_gwinflow=0.0;	
            
            }
            
            // put all dt_gwinflow into the bottom layer. 
            //this reduces the recharge spikes 
            if(dt_gwinflow <0.0){
                liq[2]=liq[2]-dt_gwinflow;
            }             

            //if water table is in the bottom layer, set Q12[1]=0
            //if water table is in the top or middle layer, set Q12[1] and Q12[0]=0
            Q12[1]=0.0;
            
            if(lindex<2){
               Q12[0]=0.0;
            }
            
                        
            if(debug==1){
                printf("lindex >0 psi_i psi_sat  kr[i]  kr[wtindex]  Ksat[i]  Ksat[wtindex]  zi  z dt_gwinflow for layer temp_recharge  \n");
                printf("%f  %f %f %f %f %f %f %f %f %f\n",	
              psi_m,  psi_sat, kr[lindex], kr[wtindex], Ksat[lindex],Ksat[wtindex],	node_depth_wt, z, dt_gwinflow, temp_recharge);
            
            }
            
            
            
        }//end AMBHAS
        /**************************************************
        Solve for Current Soil Layer Moisture, and
        Check Versus Maximum and Minimum Moisture Contents.
        **************************************************/
        
        last_index = 0;
        for (lindex = 0; lindex < options.Nlayer - 1; lindex++) {
        if (lindex == 0) {
            dt_runoff = tmp_dt_runoff[fidx];
        }
        else {
            dt_runoff = 0;
        }
        
        /* transport moisture for all sublayers **/
        
        tmp_inflow = 0.;
        
        /** Update soil layer moisture content **/
        liq[lindex] = liq[lindex] +
                      (inflow - dt_runoff) -
                      (Q12[lindex] + evap[lindex][fidx]);
        
        /** Verify that soil layer moisture is less than maximum **/
        if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
            tmp_inflow = (liq[lindex] + ice[lindex]) -
                         max_moist[lindex];
            liq[lindex] = max_moist[lindex] - ice[lindex];
        
           if (lindex == 0) {
           
    
                Q12[lindex] += tmp_inflow;
                tmp_inflow = 0;
                
             
           }
            else {
                tmplayer = lindex;
                while (tmp_inflow > 0) {
                    tmplayer--;
                    if (tmplayer < 0) {
                        /** If top layer saturated, add to runoff **/
                        runoff[fidx] += tmp_inflow;
                        tmp_inflow = 0;
                    }
                    else {
                        /** else add excess soil moisture to next higher layer **/
                        liq[tmplayer] += tmp_inflow;
                        if ((liq[tmplayer] + ice[tmplayer]) >
                            max_moist[tmplayer]) {
                            tmp_inflow =
                                ((liq[tmplayer] +
                                  ice[tmplayer]) - max_moist[tmplayer]);
                            liq[tmplayer] = max_moist[tmplayer] -
                                            ice[tmplayer];
                        }
                        else {
                            tmp_inflow = 0;
                        }
                    }
                }
            } /** end trapped excess moisture **/
        } /** end check if excess moisture in top layer **/
        
        /** verify that current layer moisture is greater than minimum **/
        if (liq[lindex] < 0) {
            /** liquid cannot fall below 0 **/
            Q12[lindex] += liq[lindex];
            liq[lindex] = 0;
        }
        if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
            /** moisture cannot fall below minimum **/
            Q12[lindex] +=
                (liq[lindex] + ice[lindex]) - resid_moist[lindex];
            liq[lindex] = resid_moist[lindex] - ice[lindex];
        }
        
        inflow = (Q12[lindex] + tmp_inflow);
        Q12[lindex] += tmp_inflow;
        
        last_index++;
    } /* end loop through soil layers */


        /**************************************************
           Compute Groundwater Recharge
        **************************************************/


  	//if the water table is below the depth of the bottom layer, use eq 6 from Niu et al. 2007
  	if((int)water_table_layer>options.Nlayer){
        lindex=options.Nlayer - 1;
        
        /** Calculate matric potential psi_m = psi_e(moist/max_moist)^(-b) [m] Campbell 1985 eq 5.9**/
        //bubble[lindex] is in cm, so the equation is divided by 100
        psi_m=bubble[lindex] /100.0 * pow((liq[lindex]/soil_con->max_moist[lindex]),
        	((-soil_con->expt[lindex]+3.0)/2.0));
          
        psi_sat=bubble[lindex]/100.0; // in m  
        
        /** Calculate recharge **/
        
        //update kr
        if ((tmp_liq = liq[lindex])  <
                        resid_moist[lindex]) {
                        tmp_liq = resid_moist[lindex];
                    }
        
        kr[lindex]=pow(((tmp_liq - resid_moist[lindex]) /
           (soil_con->max_moist[lindex] -resid_moist[lindex])),
                                          soil_con->expt[lindex]);
        
        dt_recharge=-kr[lindex]*Ksat[lindex] * ((-z-(psi_m-node_depth[lindex]))/
        	(z-node_depth[lindex]));
        if(debug==1){
        	printf("dt_recharge %f \n", dt_recharge);
        }
        
        if(RUNOFF_PRINT_FLAG==1){
        printf("layer_number_of_water_table %d dt_recharge %f gradient %f psi_sat %f psi_m %f z %f z %f K %f Ksat %f",  4, dt_recharge,  ((-z-(psi_m-node_depth[lindex]))/(z-node_depth[lindex])),psi_sat, psi_m, z, z, kr[lindex]*Ksat[lindex],Ksat[lindex]);
        }
  
        //if and recharge >1% of liq in the bottom layer, limit recharge to that max_liq~700 mm. There are instabilities when water table is close to the bottom of the soil layer
        
        if(dt_recharge>liq[lindex]*(MAX_RECHARGE_FRACTION)){
        
        /*if(RUNOFF_PRINT_FLAG==1){
        printf("max_moist_of_layer_%f_moist_of_layer_%f_layer_number_of_water_table_%d_dt_recharge_%f", soil_con->max_moist[lindex] , liq[lindex], lindex, dt_recharge );
        printf("max_moist_of_layer_%f_moist_of_layer_%f_layer_number_of_water_table_%d_dt_recharge_%f_gradient_%f_K_%f_Ksat_%f", soil_con->max_moist[lindex] , liq[lindex], lindex, dt_recharge,  ((-z-(psi_m-node_depth[lindex]))/
        	(z-node_depth[lindex])),kr[lindex]*Ksat[lindex] ,Ksat[lindex]);
   
        }*/
        //printf("\n recharge limited to %f psi_m %f, node depth %f bubble %f orig recharge %f z %f  \n",soil_con->max_moist[lindex]*0.005,  psi_m, node_depth[lindex], bubble[lindex],dt_recharge, z );
            dt_recharge=liq[lindex]*(MAX_RECHARGE_FRACTION);
        
        }
  
        //if recharge <-RF% of liq in the bottom layer, limit recharge to that
        if(dt_recharge< -liq[lindex]*(MAX_RECHARGE_FRACTION)){
        //printf("\n recharge limited to %f psi_m %f, node depth %f bubble %f orig recharge %f z %f \n",-soil_con->max_moist[lindex]*0.005,  psi_m, node_depth[lindex], bubble[lindex],dt_recharge, z );
            dt_recharge=-liq[lindex]*(MAX_RECHARGE_FRACTION);
        
        }
    }

      //if the water table is above the depth of the bottom layer, Q12 uses eq 9 from Niu et al. 2007 (see above) and dt_recharge is set to 0  
    if((int)water_table_layer<=options.Nlayer-1){
    
    //dt_recharge=0.0; // dt_recharge calculated above
    if(debug==1){
    	printf("water table is above the bottom layer %d\n", (int)water_table_layer);
    }
    
    }
      
      
    if(debug==1){
        printf("liq %f, resid_moist %f, max_moist %f, exp %f \n",liq[lindex], 
        resid_moist[lindex], soil_con->max_moist[lindex], soil_con->expt[lindex]);
        printf("Ksat %f, kr %10f, runoff_steps_per_dt %f, z %f, psi_m %f, depth_sum %f, dt_recharge %f \n",
        Ksat[lindex], kr[lindex], 
        (double)runoff_steps_per_dt, z, psi_m, depth_sum[lindex], dt_recharge);
        //printf("bubble %f, max_moist %f, liq %f, exp %f, psi_m %f, layer depth %f, porosity %f, resid_moist %f \n",bubble[lindex], soil_con->max_moist[lindex], liq[lindex], soil_con->expt[lindex], psi_m, depth[lindex],  soil_con->porosity[lindex],  soil_con->resid_moist[lindex]);
    }
 
 
    /**************************************************
       Compute Baseflow
    **************************************************/
    
    /** ARNO model for the bottom soil layer (based on bottom
        soil layer moisture from previous time step) **/
    
    lindex = options.Nlayer - 1;
    
    /** Compute relative moisture **/
    rel_moist =
        (liq[lindex] -
         resid_moist[lindex]) /
        (soil_con->max_moist[lindex] - resid_moist[lindex]);
    
    /** Baseflow is not needed with AMBHAS, so set dt baseflow to 0 **/
    dt_baseflow=0;
    
    /** Compute baseflow as function of relative moisture **/
    
    /*         frac = Dsmax * soil_con->Ds / soil_con->Ws;
    dt_baseflow = frac * rel_moist;
    if (rel_moist > soil_con->Ws) {
        frac = (rel_moist - soil_con->Ws) / (1 - soil_con->Ws);
        dt_baseflow += Dsmax * (1 - soil_con->Ds / soil_con->Ws) * pow(
            frac, soil_con->c);
    }
    */
    // Make sure baseflow isn't negative /
    if (dt_baseflow < 0) {
        dt_baseflow = 0;
    }
    
    
    /** Extract baseflow from the bottom soil layer **/
    
    /*  liq[lindex] +=
        Q12[lindex - 1] - (evap[lindex][fidx] + dt_baseflow);
     */   
        
    /** Extract recharge from the bottom soil layer **/
    
    liq[lindex] +=
        Q12[lindex - 1] - (evap[lindex][fidx] + dt_recharge);
    
    /** Check Lower Sub-Layer Moistures **/
    tmp_moist = 0;
    
    /* If soil moisture has gone below minimum, take water out
     * of baseflow and add back to soil to make up the difference
     * Note: this may lead to negative baseflow, in which case we will
     * reduce evap to make up for it */
    /*    if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
        dt_baseflow +=
            (liq[lindex] + ice[lindex]) - resid_moist[lindex];
        liq[lindex] = resid_moist[lindex] - ice[lindex];
    }
    
    if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
        // soil moisture above maximum 
        tmp_moist = ((liq[lindex] + ice[lindex]) - max_moist[lindex]);
        liq[lindex] = max_moist[lindex] - ice[lindex];
        tmplayer = lindex;
        while (tmp_moist > 0) {
            tmplayer--;
            if (tmplayer < 0) {
                // If top layer saturated, add to runoff 
                runoff[fidx] += tmp_moist;
                tmp_moist = 0;
            }
            else {
                // else if sublayer exists, add excess soil moisture /
                liq[tmplayer] += tmp_moist;
                if ((liq[tmplayer] + ice[tmplayer]) >
                    max_moist[tmplayer]) {
                    tmp_moist =
                        ((liq[tmplayer] +
                          ice[tmplayer]) - max_moist[tmplayer]);
                    liq[tmplayer] = max_moist[tmplayer] - ice[tmplayer];
                }
                else {
                    tmp_moist = 0;
                }
            }
        }
    }
    */
    
    /* If soil moisture has gone below minimum, take water out
     * of recharge and add back to soil to make up the difference
     * Note: this may lead to negative baseflow, in which case we will
     * reduce evap to make up for it */
    if ((liq[lindex] + ice[lindex]) < resid_moist[lindex]) {
        dt_recharge +=
            (liq[lindex] + ice[lindex]) - resid_moist[lindex];
        liq[lindex] = resid_moist[lindex] - ice[lindex];
    }
    
    if ((liq[lindex] + ice[lindex]) > max_moist[lindex]) {
        /* soil moisture above maximum */
        tmp_moist = ((liq[lindex] + ice[lindex]) - max_moist[lindex]);
        liq[lindex] = max_moist[lindex] - ice[lindex];
        tmplayer = lindex;
        while (tmp_moist > 0) {
            tmplayer--;
            if (tmplayer < 0) {
                /** If top layer saturated, add to runoff **/
                runoff[fidx] += tmp_moist;
                tmp_moist = 0;
            }
            else {
                /** else if sublayer exists, add excess soil moisture **/
                liq[tmplayer] += tmp_moist;
                if ((liq[tmplayer] + ice[tmplayer]) >
                    max_moist[tmplayer]) {
                    tmp_moist =
                        ((liq[tmplayer] +
                          ice[tmplayer]) - max_moist[tmplayer]);
                    liq[tmplayer] = max_moist[tmplayer] - ice[tmplayer];
                }
                else {
                    tmp_moist = 0;
                }
            }
        }
    }
    if(debug==1){   
    	  printf("dt_recharge %f, dt_gwinflow %f, temp_recharge %f, \n", dt_recharge, dt_gwinflow, temp_recharge);
    }
    baseflow[fidx] += dt_baseflow;//should be 0
    recharge[fidx] += dt_recharge+temp_recharge;
    gw_inflow[fidx] += dt_gwinflow;

    Q1[fidx] += Q12[0];
    Q2[fidx] += Q12[1];

  } /* end of sub-dt time step loop */

  /** If negative baseflow, reduce evap accordingly **/
  if (baseflow[fidx] < 0) {
      layer[lindex].evap += baseflow[fidx];
      baseflow[fidx] = 0;
  }
 
  /** Recompute Asat based on final moisture level of upper layers **/
  for (lindex = 0; lindex < options.Nlayer; lindex++) {
        tmp_moist_for_runoff[lindex] = (liq[lindex] + ice[lindex]);
        //debug=1;
        if(debug==1){
              printf("liq at end of time step: layer %d,  liq %f depth %f \n", lindex, liq[lindex], soil_con->depth[lindex] );     
            }
  }
  compute_runoff_and_asat(soil_con, tmp_moist_for_runoff, 0, &A,
                          &tmp_runoff);

  /** Store tile-wide values **/

  for (lindex = 0; lindex < options.Nlayer; lindex++) {
      layer[lindex].moist +=
          ((liq[lindex] + ice[lindex]) * frost_fract[fidx]);
  }
  cell->asat += A * frost_fract[fidx];
  cell->runoff += runoff[fidx] * frost_fract[fidx];
  cell->baseflow += baseflow[fidx] * frost_fract[fidx];
  cell->recharge += recharge[fidx] * frost_fract[fidx]+gw_inflow[fidx] * frost_fract[fidx];
  cell->gw_inflow+=0.0;
  cell->Q1+=Q1[fidx] * frost_fract[fidx];
  cell->Q2+=Q2[fidx] * frost_fract[fidx];

}
//debug=1;
 if(debug==1){
        printf("inflow %f, runoff, %f,  recharge, %f  baseflow, %f, evap, %f, D moist %f \n", 
         cell->inflow,  cell->runoff, cell->recharge,  cell->baseflow , layer[0].evap+layer[1].evap+layer[2].evap,  
         (liq_init[0]+liq_init[1]+liq_init[2]-liq[0]-liq[1]-liq[2]));
        printf("balance: inflow -runoff-recharge-baseflow-evap+soil mooisture change %f \n", 
        (cell->inflow-cell->runoff- cell->recharge- cell->baseflow -(layer[0].evap+layer[1].evap+layer[2].evap)+
        (liq_init[0]+liq_init[1]+liq_init[2]-liq[0]-liq[1]-liq[2])));
        }
        
     if (RUNOFF_PRINT_FLAG==1){
        printf("z %f , inflow %f, runoff, %f,  recharge, %f  baseflow, %f, evap, %f, D_moist %f,rel_moist_0-3 %f, %f, %f , max_moist_0-2 %f, %f, %f, evap_0-2 %f, %f, %f, q1,2 %f,  %f, layer_depth1-3, %f, %f, %f Sy %f \n ", z,
         cell->inflow,  cell->runoff, cell->recharge,  cell->baseflow , layer[0].evap+layer[1].evap+layer[2].evap,  
         (liq_init[0]+liq_init[1]+liq_init[2]-liq[0]-liq[1]-liq[2]), liq[0]/max_moist[0],
    liq[1]/max_moist[1],liq[2]/max_moist[2], max_moist[1], max_moist[0], max_moist[2], layer[0].evap, layer[1].evap, layer[2].evap,cell->Q1, cell->Q2, depth[0],depth[1],depth[2], Sy );
    
        }
        
        
/** Compute water table depth **/
wrap_compute_zwt(soil_con, cell);

/** Recompute Thermal Parameters Based on New Moisture Distribution **/
if (options.FULL_ENERGY || options.FROZEN_SOIL) {
  for (lindex = 0; lindex < options.Nlayer; lindex++) {
      tmp_layer = cell->layer[lindex];
      moist[lindex] = tmp_layer.moist;
  }

  ErrorFlag = distribute_node_moisture_properties(energy->moist,
                                                  energy->ice,
                                                  energy->kappa_node,
                                                  energy->Cs_node,
                                                  soil_con->Zsum_node,
                                                  energy->T,
                                                  soil_con->max_moist_node,
                                                  soil_con->expt_node,
                                                  soil_con->bubble_node,
                                                  moist, soil_con->depth,
                                                  soil_con->soil_dens_min,
                                                  soil_con->bulk_dens_min,
                                                  soil_con->quartz,
                                                  soil_con->soil_density,
                                                  soil_con->bulk_density,
                                                  soil_con->organic, Nnodes,
                                                  options.Nlayer,
                                                  soil_con->FS_ACTIVE);
  if (ErrorFlag == ERROR) {
      return (ERROR);
  }
}
return (0);
}

/******************************************************************************
* @brief    Calculate the saturated area and runoff
******************************************************************************/
void
compute_runoff_and_asat(soil_con_struct *soil_con,
                        double          *moist,
                        double           inflow,
                        double          *A,
                        double          *runoff)
{
    extern option_struct options;
    double               top_moist; // total moisture (liquid and frozen) in topmost soil layers (mm)
    double               top_max_moist; // maximum storable moisture (liquid and frozen) in topmost soil layers (mm)
    size_t               lindex;
    double               ex;
    double               max_infil;
    double               i_0;
    double               basis;

    top_moist = 0.;
    top_max_moist = 0.;
    for (lindex = 0; lindex < options.Nlayer - 1; lindex++) {
        top_moist += moist[lindex];
        top_max_moist += soil_con->max_moist[lindex];
    }
    if (top_moist > top_max_moist) {
        top_moist = top_max_moist;
    }

    /** A as in Wood et al. in JGR 97, D3, 1992 equation (1) **/
    ex = soil_con->b_infilt / (1.0 + soil_con->b_infilt);
    *A = 1.0 - pow((1.0 - top_moist / top_max_moist), ex);

    max_infil = (1.0 + soil_con->b_infilt) * top_max_moist;
    i_0 = max_infil * (1.0 - pow((1.0 - *A), (1.0 / soil_con->b_infilt)));

    /** equation (3a) Wood et al. **/

    if (inflow == 0.0) {
        *runoff = 0.0;
    }
    else if (max_infil == 0.0) {
        *runoff = inflow;
    }
    else if ((i_0 + inflow) > max_infil) {
        *runoff = inflow - top_max_moist + top_moist;
    }
    /** equation (3b) Wood et al. (wrong in paper) **/
    else {
        basis = 1.0 - (i_0 + inflow) / max_infil;
        *runoff = (inflow - top_max_moist + top_moist +
                   top_max_moist *
                   pow(basis, 1.0 * (1.0 + soil_con->b_infilt)));
    }
    if (*runoff < 0.) {
        *runoff = 0.;
    }
}