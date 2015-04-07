#ifndef _MAIN_
#define _MAIN_

#include <iostream>
#include <string.h>

void displayHelp(){
	std::cout << "This is the help - kind of like a man page" << std::endl << std::endl;

	std::cout << "add Device to simulator library" << std::endl
			  << "usage:" << std::endl << "\t./drachma -d DEVICE_FILE [DEVICE_NAME]"
			  << std::endl << std::endl;

	std::cout << "Help user - print usage synopsis" << std::endl
			  << "usage:" << std::endl << "\t./drachma -h"
			  << std::endl << std::endl;

	std::cout << "list objects contained in the simulator Library" << std::endl
			  << "usage:" << std::endl << "\t./drachma -l"
			  << std::endl << std::endl;

	std::cout << "add Memory and replacement algorithm configurations" << std::endl
			  << "usage:" << std::endl << "\t./drachma -m MEMORY_FILE [CONFIGURATION_NAME]"
			  << std::endl << std::endl;

	std::cout << "start Simulation" << std::endl
			  << "usage:" << std::endl << "\t./drachma -s DEVICE_NAME CONFIGURATION_NAME APPLICATION_NAME"
			  << std::endl << std::endl;

	std::cout << "add application Trace to simulator library" << std::endl
			  << "usage:" << std::endl << "\t./drachma -t TRACE_FILE [APPLICATION_NAME]"
			  << std::endl << std::endl;
}

int main(int argc, char** argv){

	std::cout << std::endl << "Drachma : Reconfigurable Computing Cache Simulator" << std::endl << std::endl;

    if(argc > 1 && argv[1][0] == 0x2D && argv[1][1] != 0){ //switches present

        // read for switch data
        short int switch_count = strlen(argv[1]) - 1;
        char switch_choices[switch_count];
        bool bad_switch = false;

    	for(int i = 0; i < switch_count; i++)
    		switch_choices[i] = argv[1][i + 1];

    	for(int i = 0; i < switch_count; i++){
    		switch(switch_choices[i]){

    			case 'd':
    				/*
    				 * add device to simulator library
    				 *
    				 * usage:
    				 * 		./drachma -d DEVICE_FILE [DEVICE_NAME]
    				 */
    				goto exit;

    			case 'h': // help user
    				/*
    				 * help user - print usage synopsis
    				 *
    				 * usage:
    				 * 		./drachma -h
    				 */
    				displayHelp();
    				break;

    			case 'l':
    				/*
    				 * list objects contained in the simulator library
    				 *
    				 * usage:
    				 * 		./drachma -l
    				 */
    				break;

    			case 'm':
    				/*
    				 * add memory and replacement algorithm configurations
    				 *
    				 * usage:
    				 * 		./drachma -m MEMORY_FILE [CONFIGURATION_NAME]
    				 */
    				goto exit;

    			case 's':
    				/*
    				 * start simulation
    				 *
    				 * usage:
    				 * 		./drachma -s DEVICE_NAME CONFIGURATION_NAME APPLICATION_NAME
    				 */
    				goto exit;

    			case 't':
    				/*
    				 * add application trace to simulator library
    				 *
    				 * usage:
    				 * 		./drachma -t TRACE_FILE [APPLICATION_NAME]
    				 */
    				goto exit;

    			default: // unrecognized switch
					bad_switch = true;
    		}
    	}

    }else{ // no switches - default action

    	std::cout << ">>> No Switches Present!  Have some help:" << std::endl << std::endl;
    	displayHelp();
    }

    exit:
		return 0;
}

#endif
