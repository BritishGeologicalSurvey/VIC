#ifndef NC_LIBS
#NC_LIBS = $(shell nc-config --libs)
#endif
#$(info $$NC_LIBS is [${NC_LIBS}])

#LIBRARY = -lm ${NC_LIBS}

AMBHAS : main.o calculateGWDynamicSS.o calculateGWFlow.o calculateGwInit.o calculateGWSS.o getObsDataCount.o GW_initialise.o GW_read_data.o GW_read_h_ini.o GW_read_ts.o GW_write_output.o readGlobalData.o writeObsBH.o link_Ambhas-VIC_Domain.o updateTrans.o

	gcc -o  AMBHAS main.o calculateGWDynamicSS.o calculateGWFlow.o calculateGwInit.o calculateGWSS.o getObsDataCount.o GW_initialise.o GW_read_data.o GW_read_h_ini.o GW_read_ts.o GW_write_output.o readGlobalData.o writeObsBH.o link_Ambhas-VIC_Domain.o updateTrans.o -lm -lnetcdf

main.o: main.c
	gcc -c main.c

calculateGWDynamicSS.o: calculateGWDynamicSS.c
	gcc -c calculateGWDynamicSS.c

calculateGWFlow.o: calculateGWFlow.c
	gcc -c -lm calculateGWFlow.c

calculateGwInit.o: calculateGwInit.c
	gcc -c calculateGwInit.c

calculateGWSS.o: calculateGWSS.c
	gcc -c calculateGWSS.c

getObsDataCount.o: getObsDataCount.c
	gcc -c getObsDataCount.c

GW_read_data.o: GW_read_data.c
	gcc -c GW_read_data.c 

GW_read_h_ini.o: GW_read_h_ini.c
	gcc -c GW_read_h_ini.c

GW_read_ts.o:  GW_read_ts.c
	gcc -c GW_read_ts.c 

GW_write_output.o: GW_write_output.c
	gcc -c GW_write_output.c 

readGlobalData.o: readGlobalData.c
	gcc -c readGlobalData.c

writeObsBH.o: writeObsBH.c
	gcc -c writeObsBH.c

link_Ambhas-VIC_Domain.o: link_Ambhas-VIC_Domain.c
	gcc -c link_Ambhas-VIC_Domain.c

updateTrans.o: updateTrans.c
	gcc -c updateTrans.c

GW_initialise.o: GW_initialise.c
	gcc -c GW_initialise.c

clean:
	rm main.o calculateGWDynamicSS.o calculateGWFlow.o calculateGwInit.o calculateGWSS.o getObsDataCount.o GW_initialise.o GW_read_data.o GW_read_h_ini.o GW_read_ts.o GW_write_output.o readGlobalData.o writeObsBH.o updateTrans.o
