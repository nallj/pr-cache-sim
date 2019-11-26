#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>

#include "wallet.h"
#include "device.h"
#include "fileHandler.h"

#include "storage/reconfigurableRegions.h"
#include "storage/module.h"


bool fileExists(const std::string& file) {
	return ( access(file.c_str(), F_OK) != -1 );
}

void deviceHelp() {
	// origin:	drachma -d DEVICE_FILE [DEVICE_NAME]

	std::cout << "add Device to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -da DEVICE_FILE" << std::endl
			  << "remove Device from simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -dr DEVICE_FILE"
			  << std::endl << std::endl;
}

void memoryHelp() {
	// origin:	drachma -m MEMORY_FILE [CONFIGURATION_NAME]

	std::cout << "add Memory and Replacement Algorithm Configurations" << std::endl
			  << "usage:" << std::endl << "\t./drachma -ma MEMORY_FILE" << std::endl
			  << "remove Memory and Replacement Algorithm Configurations" << std::endl
			  << "usage:" << std::endl << "\t./drachma -mr MEMORY_FILE" << std::endl
			  << std::endl << std::endl;
}

void applicationHelp() {
	// origin:	drachma -a APPLICATION_FILE [APPLICATION_NAME]

	std::cout << "add Application to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -aa APPLICATION_FILE" << std::endl
			  << "remove Application from simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -ar APPLICATION_FILE"
			  << std::endl << std::endl;
}

void traceHelp() {
	// origin:	drachma -t TRACE_FILE [APPLICATION_NAME]

	std::cout << "add application Trace File to simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -ta TRACE_FILE" << std::endl
			  << "remove application Trace File from simulator wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -tr TRACE_FILE" << std::endl
			  << std::endl << std::endl;
}

void startHelp() {
	// origin:	drachma -s MEMORY_ID APPLICATION_ID TRACE_ID [STOP_AT_CC]

	std::cout << "start Simulation" << std::endl
			  << "usage:" << std::endl << "\t./drachma -s MEMORY_ID APPLICATION_ID TRACE_ID [STOP_AT_CC]"
			  << std::endl << std::endl;
}

void displayHelp() {
	std::cout << "This is the help - kind of like a man page" << std::endl << std::endl;

	//deviceHelp();

	std::cout << "Help user - print usage synopsis" << std::endl
			  << "usage:" << std::endl << "\t./drachma -h"
			  << std::endl << std::endl;

	memoryHelp();

	applicationHelp();

	traceHelp();

	std::cout << "list objects contained in the simulator Wallet" << std::endl
			  << "usage:" << std::endl << "\t./drachma -w"
			  << std::endl << std::endl;

	startHelp();
}


int main(int argc, char** argv) {

	// clear screen
	std::cout << "\033[2J\033[1;1H";

	std::cout <<  "Drachma : Partial Reconfiguration Cache Simulator\n\n\n";

	// instantiate drachma wallet
	wallet library = wallet();

	// switches present
	if (argc > 1 && argv[1][0] == 0x2D && argv[1][1] != 0) {

		// read for switch data
		short int switch_count = strlen(argv[1]) - 1;
		char switch_choices[switch_count];
		bool bad_switch = false;

		for (int i = 0; i < switch_count; i++) {
			switch_choices[i] = argv[1][i + 1];
		}

		for (int i = 0; i < switch_count; i++) {

			bool valid_use = false;

			switch(switch_choices[i]) {

				//case 'd': 
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

					/*if (switch_count == 2) { // require two switches

						if (switch_choices[1] == 'a') {
							std::cout << "Adding new device '" << argv[2] << "' to the Wallet.\n\n";
							valid_use = true;

							if (fileExists(argv[2])) {

								std::vector<std::string> device_params {"family", "model", "slices"};
								fileHandler device_reader = fileHandler(argv[2], device_params);

								if (device_reader.isFileValid()) {

								library.addDevice(argv[2]);
								std::cout << "Done!\n";

								}else
									std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Device file.\n\n";
							}else
							std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

						}else if (switch_choices[1] == 'r') {
							std::cout << "Removing device '" << argv[2] << "' from the Wallet.\n\n";
							valid_use = true;

							if (!library.removeDevice(argv[2]))
								std::cout << "ERROR: Could not remove Device '" << argv[2] << "' from the Wallet."
										<< " <Device was Not Present>\n\n";

						}
					}

					if (!valid_use) {
						std::cout << "ERROR: Improper switch use; The operations concerning Devices are as follows:\n";
						deviceHelp();
					}

					goto exit;
				*/

				case 'h': // help user
					/*
						* help user - print usage synopsis
						*
						* usage:
						* 		./drachma -h
						*/
					displayHelp();
					break;

				case 'm':
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

					if (switch_count == 2) { // require two switches

						if (switch_choices[1] == 'a') {
							std::cout << "Adding new Memory Configuration '" << argv[2] << "' to the Wallet.\n\n";
								valid_use = true;

								if (fileExists(argv[2])) {

									fileHandler memory_reader = fileHandler(argv[2],
											library.getMemoryParamRules(), library.getMemoryRegexRules());

								if (memory_reader.isFileValid()) {
									library.addMemory(argv[2]);

									std::cout << "Done!\n";
								}else
									std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Memory Configuration file.\n\n";

								}else
									std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

						}else if (switch_choices[1] == 'r') {
							std::cout << "Removing Memory Configuration '" << argv[2] << "' from the Wallet.\n\n";
							valid_use = true;

							if (!library.removeMemory(argv[2]))
								std::cout << "ERROR: Could not remove Memory Configuration '" << argv[2] << "' from the Wallet."
										<< " <Memory Configuration was Not Present>\n\n";
						}
					}

					if (!valid_use) {
						std::cout << "ERROR: Improper switch use; The operations concerning Memory Configuration files are as follows:\n";
						memoryHelp();
					}

					goto exit;

				case 'a':
					/*
						* add application to simulator wallet
						*
						* usage:
						* 		./drachma -aa APPLICATION_FILE [APPLICATION_NAME]
						*
						* remove application to simulator wallet
						*
						* usage:
						* 		./drachma -ar APPLICATION_FILE [APPLICATION_NAME]
						*/

					if (switch_count == 2) { // require two switches

					if (switch_choices[1] == 'a') {
						std::cout << "Adding new application '" << argv[2] << "' to the Wallet.\n\n";
						valid_use = true;

						if (fileExists(argv[2])) {

							fileHandler application_reader = fileHandler(argv[2],
									library.getApplicationParamRules(), library.getApplicationRegexRules());

							if (application_reader.isFileValid()) {
								library.addApplication(argv[2]);

								std::cout << "Done!\n";
							}else
								std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Application file.\n\n";
						}else
							std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

					}else if (switch_choices[1] == 'r') {
						std::cout << "Removing application '" << argv[2] << "' from the Wallet.\n\n";
						valid_use = true;

						if (!library.removeApplication(argv[2]))
							std::cout << "ERROR: Could not remove Application '" << argv[2] << "' from the Wallet."
										<< " <Application was Not Present>\n\n";

					}
				

				if (!valid_use) {
					std::cout << "ERROR: Improper switch use; The operations concerning Application files are as follows:\n";
					deviceHelp();
				}

				goto exit;
				}

				case 't':
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

					if (switch_count == 2) { // require two switches

						if (switch_choices[1] == 'a') {
							std::cout << "Adding new Trace File '" << argv[2] << "' to the Wallet.\n\n";
						valid_use = true;

						if (fileExists(argv[2])) {

							fileHandler trace_reader = fileHandler(argv[2],
									library.getTraceParamRules(), library.getTraceRegexRules());

								if (trace_reader.isFileValid()) {
									library.addTraceFile(argv[2]);

									std::cout << "Done!\n";
								}else
									std::cout << "ERROR: The target file '" << argv[2] << "' is not a valid Trace File.\n\n";

						}else
							std::cout << "ERROR: The target file '" << argv[2] << "' does not exist.\n\n";

						}else if (switch_choices[1] == 'r') {
							std::cout << "Removing Trace File '" << argv[2] << "' from the Wallet.\n\n";
							valid_use = true;

							if (!library.removeTraceFile(argv[2]))
								std::cout << "ERROR: Could not remove Trace File '" << argv[2] << "' from the Wallet."
										<< " <Trace File was Not Present>\n\n";
						}
					}

					if (!valid_use) {
						std::cout << "ERROR: Improper switch use; The operations concerning Trace files are as follows:\n";
						traceHelp();
					}

					goto exit;

				case 'w':
					/*
						* list objects contained in the simulator wallet"rcr count"
						*
						* usage:
						* 		./drachma -w
						*/

					library.printDetails();
					goto exit;

				case 's':
					/*
						* start simulation
						*
						* usage:
						* 		./drachma -s MEMORY_ID APPLICATION_ID TRACE_ID
						*/

					if (argc == 5 || argc == 6) {

						// initialize the simulation device
						//device sim_device = library.getDevice( atoi(argv[2]) );
						device sim_device = device();

						// get memory hierarchy
						reconfigurableRegions memory = library.getMemory( atoi(argv[2]) );

						// get application
						application* app = library.getApplication( atoi(argv[3]) );

						// get application traces
						std::vector<std::string> traces = library.getTraceFile( atoi(argv[4]) );

						// apply memory hierarchy to device
						sim_device.associateHierarchy(memory);

						// prepare device resources needed by the application
						if (sim_device.prepareApplicationResources(app)) {

							sim_device.parseTraceFile(traces);

							// if specified, stop the simulation after a certain mount of clock cycles
							if (argc == 5) {
								sim_device.simulateApplication();
							} else {
								sim_device.simulateApplication( atoi(argv[5]) );
							}

						} else {
							std::cout << "ERROR: Chosen application's bitstream library is too large to fit in the selected memory hierarchy.\n";
						}

					} else {
						std::cout << "ERROR: Improper switch use; The operations concerning Starting the Simulator are as follows:\n";
						startHelp();
					}

					goto exit;

				default: // unrecognized switch
				bad_switch = true;
			}
		}

	} else { // no switches - default action

		std::cout << ">>> No Switches Present!  Have some help:" << std::endl << std::endl;
		displayHelp();
	}

	exit:
	std::cout << std::endl;
	return 0;
}
