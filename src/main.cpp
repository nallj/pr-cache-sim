#ifndef _MAIN_
#define _MAIN_

#include <iostream>
#include <string>
#include <cstring>

#include "wallet.h"
#include "device.h"
#include "reader.h"

void displayHelp(){
	std::cout << "This is the help - kind of like a man page" << std::endl << std::endl;

	std::cout << "add Device to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -d DEVICE_FILE [DEVICE_NAME]"
			  << std::endl << std::endl;

	std::cout << "Help user - print usage synopsis" << std::endl
			  << "usage:" << std::endl << "\t./drachma -h"
			  << std::endl << std::endl;

	std::cout << "add Memory and replacement algorithm configurations" << std::endl
			  << "usage:" << std::endl << "\t./drachma -m MEMORY_FILE [CONFIGURATION_NAME]"
			  << std::endl << std::endl;

	std::cout << "start Simulation" << std::endl
			  << "usage:" << std::endl << "\t./drachma -s DEVICE_NAME CONFIGURATION_NAME APPLICATION_NAME"
			  << std::endl << std::endl;

	std::cout << "add application Trace to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -t TRACE_FILE [APPLICATION_NAME]"
			  << std::endl << std::endl;

	std::cout << "list objects contained in the simulator Wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -w"
			  << std::endl << std::endl;
}

int main(int argc, char** argv){

	std::cout << std::endl << "Drachma : Reconfigurable Computing Cache Simulator" << std::endl << std::endl;

	// instantiate drachma wallet
	wallet library = wallet();
	//if(library)


    if(argc > 1 && argv[1][0] == 0x2D && argv[1][1] != 0){ //switches present

        // read for switch data
        short int switch_count = strlen(argv[1]) - 1;
        char switch_choices[switch_count];
        bool bad_switch = false;

    	for(int i = 0; i < switch_count; i++)
    		switch_choices[i] = argv[1][i + 1];

    	for(int i = 0; i < switch_count; i++){

    		switch(switch_choices[i]){

    			case 'd': {
    				/*
    				 * add device to simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -d DEVICE_FILE [DEVICE_NAME]
    				 */

    				std::vector<std::string> device_args {"family", "model", "slices"};

    				reader device_reader = reader("device.dev", device_args);

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
    				 * 		./drachma -m MEMORY_FILE [CONFIGURATION_NAME]
    				 */

    				std::vector<std::string> memory_params {"name", "main name"};
    				std::vector<std::string> regex_filter {"[l][1-9] \\bname\\b[:] [\\w ]+"};

    				reader memory_reader = reader("memory.mem", memory_params, regex_filter);

    				goto exit;
    			}

    			case 's': {
    				/*
    				 * start simulation
    				 *
    				 * usage:
    				 * 		./drachma -s DEVICE_NAME CONFIGURATION_NAME APPLICATION_NAME
    				 */

    				goto exit;
    			}

    			case 't': {
    				/*
    				 * add application trace to simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -t TRACE_FILE [APPLICATION_NAME]
    				 */

    				std::vector<std::string> trace_params {"name"};
    				std::vector<std::string> regex_filter {"\\d+"};

    				reader trace_reader = reader("traces.trc", trace_params, regex_filter);

    				goto exit;
    			}

    			case 'w': {
    				/*
    				 * list objects contained in the simulator wallet
    				 *
    				 * usage:
    				 * 		./drachma -w
    				 */

    				std::vector<std::string> wallet_params {"name", "d", "m", "t"};

    				reader trace_reader = reader("wallet.wal", wallet_params);

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
		std::cout << std::endl << std::endl << std::endl;
		return 0;
}

#endif
