#ifndef _MAIN_
#define _MAIN_

#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>

#include "wallet.h"
#include "device.h"
#include "fileHandler.h"


bool fileExists(const std::string& file){
	return ( access(file.c_str(), F_OK) != -1 );
}

void deviceHelp(){
	// origin:	drachma -d DEVICE_FILE [DEVICE_NAME]

	std::cout << "add Device to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -da DEVICE_FILE" << std::endl
			  << "remove Device from simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -dr DEVICE_FILE"
			  << std::endl << std::endl;
}

void memoryHelp(){
	// origin:	drachma -m MEMORY_FILE [CONFIGURATION_NAME]

	std::cout << "add Memory and Replacement Algorithm Configurations" << std::endl
			  << "usage:" << std::endl << "\t./drachma -ma MEMORY_FILE" << std::endl
			  << "remove Memory and Replacement Algorithm Configurations" << std::endl
			  << "usage:" << std::endl << "\t./drachma -mr MEMORY_FILE" << std::endl
			  << std::endl << std::endl;
}

void traceHelp(){
	// origin:	drachma -t TRACE_FILE [APPLICATION_NAME]

	std::cout << "add application Trace File to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -ta TRACE_FILE" << std::endl
			  << "remove application Trace File from simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -tr TRACE_FILE" << std::endl
			  << std::endl << std::endl;
}

void startHelp(){
	// origin:	drachma -s DEVICE_NAME CONFIGURATION_NAME APPLICATION_NAME

	std::cout << "start Simulation" << std::endl
			  << "usage:" << std::endl << "\t./drachma -s DEVICE_ID MEMORY_ID TRACE_ID STOP_AT_CC"
			  << std::endl << std::endl;
}

void displayHelp(){
	std::cout << "This is the help - kind of like a man page" << std::endl << std::endl;

	deviceHelp();

	std::cout << "Help user - print usage synopsis" << std::endl
			  << "usage:" << std::endl << "\t./drachma -h"
			  << std::endl << std::endl;

	memoryHelp();

	startHelp();

	traceHelp();

	std::cout << "list objects contained in the simulator Wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -w"
			  << std::endl << std::endl;
}


int main(int argc, char** argv){

	std::cout << std::endl << "Drachma : Reconfigurable Computing Cache Simulator\n\n\n";

	// instantiate drachma wallet
	wallet library = wallet();

    if(argc > 1 && argv[1][0] == 0x2D && argv[1][1] != 0){ //switches present

        // read for switch data
        short int switch_count = strlen(argv[1]) - 1;
        char switch_choices[switch_count];
        bool bad_switch = false;

    	for(int i = 0; i < switch_count; i++)
    		switch_choices[i] = argv[1][i + 1];

    	for(int i = 0; i < switch_count; i++){

    		bool valid_use = false;

    		switch(switch_choices[i]){

    			case 'd': {
    				/*
    				 * add device to simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -da DEVICE_FILE [DEVICE_NAME]
    				 *
    				 * remove device to simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -dr DEVICE_FILE [DEVICE_NAME]
    				 */

    				if(switch_count == 2){ // require two switches

    					if(switch_choices[1] == 'a'){
    						std::cout << "Adding new device '" << argv[2] << "' to the Wallet.\n\n";
    						valid_use = true;

    						if(fileExists(argv[2])){

    							std::vector<std::string> device_params {"family", "model", "slices"};
    							fileHandler device_reader = fileHandler(argv[2], device_params);

    							if(device_reader.isFileValid()){

    							library.addDevice(argv[2]);
    							std::cout << "Done!\n";

    							}else
    								std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Device file.\n\n";
    						}else
								std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

    					}else if(switch_choices[1] == 'r'){
    						std::cout << "Removing device '" << argv[2] << "' from the Wallet.\n\n";
    						valid_use = true;

    						if(!library.removeDevice(argv[2]))
    							std::cout << "ERROR: Could not remove Device '" << argv[2] << "' from the Wallet."
										  << " <Device was Not Present>\n\n";

    					}
    				}

    				if(!valid_use){
    					std::cout << "ERROR: Improper switch use; The operations concerning Devices are as followed:\n";
    					deviceHelp();
    				}

    				goto exit;
    			}

    			case 'h': { // help user
    				/*
    				 * help user - print usage synopsis
    				 *
    				 * usage:
    				 * 		./drachma -h
    				 */
    				displayHelp();
    				break;
    			}

    			case 'm': {
    				/*
    				 * add memory and replacement algorithm configurations
    				 *
    				 * usage:
    				 * 		./drachma -ma MEMORY_FILE [CONFIGURATION_NAME]
    				 *
     				 * remove memory and replacement algorithm configurations
    				 *
    				 * usage:
    				 * 		./drachma -mr MEMORY_FILE [CONFIGURATION_NAME]
    				 */

    				if(switch_count == 2){ // require two switches

    					if(switch_choices[1] == 'a'){
    						std::cout << "Adding new Memory Configuration '" << argv[2] << "' to the Wallet.\n\n";
    	    				valid_use = true;

    	    				if(fileExists(argv[2])){

    	    					std::vector<std::string> memory_params {"name", "main name", "main size", "main read latency", "main search latency"};
    	    					std::vector<std::string> regex_filter {"[l][1-9] \\bname\\b[:] [\\w ]+",
    	    															"[l][1-9] \\bsize\\b[:] [\\w ]+",
																		"[l][1-9] \\b(read|search)\\b \\blatency\\b[:] [\\w ]+"};

    	    					fileHandler memory_reader = fileHandler(argv[2], memory_params, regex_filter);

    							if(memory_reader.isFileValid()){

    								library.addMemory(argv[2]);
    								std::cout << "Done!\n";
    							}else
    								std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Memory Configuration file.\n\n";

    	    				}else
    	    					std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

    					}else if(switch_choices[1] == 'r'){
    						std::cout << "Removing Memory Configuration '" << argv[2] << "' from the Wallet.\n\n";
    						valid_use = true;

    						if(!library.removeMemory(argv[2]))
    							std::cout << "ERROR: Could not remove Memory Configuration '" << argv[2] << "' from the Wallet."
										  << " <Memory Configuration was Not Present>\n\n";
    					}
    				}

    				if(!valid_use){
    					std::cout << "ERROR: Improper switch use; The operations concerning Memory Configurations are as followed:\n";
    					memoryHelp();
    				}

    				goto exit;
    			}

    			case 's': {
    				/*
    				 * start simulation
    				 *
    				 * usage:
    				 * 		./drachma -s DEVICE_NAME CONFIGURATION_NAME APPLICATION_NAME
    				 */

    				if(argc == 5 || argc == 6){

    					device sim_device = library.getDevice( atoi(argv[2]) );
    					storageUnit memory = library.getMemory( atoi(argv[3]) );
    					std::vector<std::string> trace_file = library.getTraceFile( atoi(argv[4]) );

    					sim_device.parseModules(memory); // apply memory to device

    					if(argc == 5)
    						sim_device.simulateApplication(trace_file); // apply traces to device (simulate)
    					else
    						sim_device.simulateApplication(trace_file, atoi(argv[5])); // apply traces to device until stopping trace is reached

    				}else{
    					std::cout << "ERROR: Improper switch use; The operations concerning Starting the Simulator are as followed:\n";
    					startHelp();
    				}

    				goto exit;
    			}

    			case 't': {
    				/*
    				 * add application trace to simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -ta TRACE_FILE [APPLICATION_NAME]
    				 *
    				 * remove application trace from simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -tr TRACE_FILE [APPLICATION_NAME]
    				 */

    				if(switch_count == 2){ // require two switches

    					if(switch_choices[1] == 'a'){
    						std::cout << "Adding new Trace File '" << argv[2] << "' to the Wallet.\n\n";
							valid_use = true;

							if(fileExists(argv[2])){

								std::vector<std::string> trace_params {"name"};
    							std::vector<std::string> regex_filter {"\\d+"};

    							fileHandler trace_reader = fileHandler(argv[2], trace_params, regex_filter);

    							if(trace_reader.isFileValid()){

    								library.addTraceFile(argv[2]);

    								std::cout << "Done!\n";
    							}else
    								std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Trace File.\n\n";

							}else
								std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

    					}else if(switch_choices[1] == 'r'){
    						std::cout << "Removing Trace File '" << argv[2] << "' from the Wallet.\n\n";
    						valid_use = true;

    						if(!library.removeTraceFile(argv[2]))
    							std::cout << "ERROR: Could not remove Trace File '" << argv[2] << "' from the Wallet."
										  << " <Trace File was Not Present>\n\n";
    					}
    				}

    				if(!valid_use){
    					std::cout << "ERROR: Improper switch use; The operations concerning Trace File are as followed:\n";
    					traceHelp();
    				}

    				goto exit;
    			}

    			case 'w': {
    				/*
    				 * list objects contained in the simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -w
    				 */

    				library.printDetails();
    				goto exit;
    			}

    			default: // unrecognized switch
					bad_switch = true;
    		}
    	}

    }else{ // no switches - default action

    	std::cout << ">>> No Switches Present!  Have some help:" << std::endl << std::endl;
    	displayHelp();
    }

    exit:
		std::cout << std::endl;
		return 0;
}

#endif
