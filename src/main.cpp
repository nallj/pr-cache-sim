#include <cstring> // strlen
#include <iostream> // cout
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <string> // string
// #include <sys/stat.h>
#include <unistd.h> // access

#include "device.hpp"
#include "fileHandler.hpp"
#include "userError.hpp"
#include "wallet.hpp"

#include "storage/reconfigurableRegions.hpp"
#include "storage/rrModule.hpp"

bool fileExists(const std::string& file) {
  return access(file.c_str(), F_OK) != -1;
}

void deviceHelp() {
  // origin:  drachma -d DEVICE_FILE [DEVICE_NAME]

  std::cout << "Add device to simulator wallet.\n"
    << "usage:\n"
    << "\t./drachma -da DEVICE_FILE\n"
    << "Remove Device from simulator wallet.\n"
    << "usage:\n"
    << "\t./drachma -dr DEVICE_FILE\n\n";
}

void memoryHelp() {
  // origin:  drachma -m MEMORY_FILE [CONFIGURATION_NAME]

  std::cout << "Add memory and replacement algorithm configurations.\n"
    << "usage:\n"
    << "\t./drachma -ma MEMORY_FILE\n"
    << "Remove memory and replacement algorithm configurations.\n"
    << "usage:\n"
    << "\t./drachma -mr MEMORY_FILE\n\n";
}

void applicationHelp() {
  // origin:  drachma -a APPLICATION_FILE [APPLICATION_NAME]

  std::cout << "Add application to simulator wallet.\n"
    << "usage:\n"
    << "\t./drachma -aa APPLICATION_FILE\n"
    << "Remove application from simulator wallet.\n"
    << "usage:\n"
    << "\t./drachma -ar APPLICATION_FILE\n\n";
}

void taskGraphHelp() {
  // origin:  drachma -t TASK_GRAPH_FILE [APPLICATION_NAME]

  std::cout << "Add application task graph file to simulator wallet.\n"
    << "usage:\n"
    << "\t./drachma -ta TASK_GRAPH_FILE\n"
    << "Remove application task graph file from simulator wallet.\n"
    << "usage:\n"
    << "\t./drachma -tr TASK_GRAPH_FILE\n\n";
}

void startHelp() {
  // origin:  drachma -s MEMORY_ID APPLICATION_ID TASK_GRAPH_ID [STOP_AT_CC]

  std::cout << "Start simulation.\n"
    << "usage:\n"
    << "\t./drachma -s MEMORY_ID APPLICATION_ID TASK_GRAPH_ID [STOP_AT_CC]\n\n";
}

void displayHelp() {
  std::cout << "> How to use Drachma\n\n";

  std::cout << "Help user - print usage synopsis\n"
    << "usage:\n"
    << "\t./drachma -h\n\n";

  memoryHelp();
  applicationHelp();
  taskGraphHelp();

  std::cout << "List objects contained in the simulator wallet\n"
    << "usage:\n"
    << "\t./drachma -w\n\n";

  startHelp();
}


int main(int argc, char** argv) {

  // Clear screen.
  // TODO: Remove someday.
  std::cout << "\033[2J\033[1;1H";

  std::cout << "Drachma : Partial Reconfiguration Cache Simulator\n\n\n";
  
  // try {
    // Ensure switches are present.
    if (argc < 2 || argv[1][0] != 0x2D || argv[1][1] == 0) {
      throw userError("No switches provided.", displayHelp);
    }

    // Instantiate Drachma wallet.
    wallet library;

    // Read switch data.
    const auto switch_count = std::strlen(argv[1]) - 1;
    char switch_choices[switch_count];
    auto bad_switch = false;

    for (auto i = 0; i < switch_count; i++) {
      switch_choices[i] = argv[1][i + 1];
    }

    for (auto i = 0; i < switch_count; i++) {
      auto valid_use = false;

      switch (switch_choices[i]) {
        case 'h':
          /*
           * Help user - print usage synopsis
           * usage:
           *     ./drachma -h
           */
          displayHelp();
          break;

        case 'm':
          /*
           * Add memory and replacement algorithm configurations.
           * usage:
           *     ./drachma -ma MEMORY_FILE [CONFIGURATION_NAME]
           *
           * Remove memory and replacement algorithm configurations.
           * usage:
           *     ./drachma -mr MEMORY_FILE [CONFIGURATION_NAME]
           */

          // Require two switches.
          if (switch_count == 2) {
            const auto memory_file = argv[2];

            const auto second_switch = switch_choices[1];
            if (second_switch == 'a') {
              std::cout << "Adding new memory configuration '" << memory_file << "' to the wallet.\n\n";
              valid_use = true;

              if (fileExists(memory_file)) {
                const auto memory_reader = fileHandler(
                  memory_file,
                  library.getMemoryParamRules(),
                  library.getMemoryRegexRules()
                );

                if (memory_reader.isFileValid()) {
                  library.addMemory(memory_file);

                  std::cout << "Done!\n";
                } else {
                  const auto msg = "The target file '" + std::string(memory_file) + "' is not a valid memory configuration file.";
                  throw userError(msg);
                }

              } else {
                const auto msg = "The target file '" + std::string(memory_file) + "' does not exist.";
                throw userError(msg);
              }

            } else if (second_switch == 'r') {
              std::cout << "Removing memory configuration '" << memory_file << "' from the wallet.\n\n";
              valid_use = true;

              if (!library.removeMemory(memory_file)) {
                const auto msg = "Could not remove memory configuration '" + std::string(memory_file) + "' from the wallet. Memory configuration not present.";
                throw userError(msg);
              }
            }
          }

          if (!valid_use) {
            const auto msg = "Improper switch use. The operations concerning Memory Configuration files are as follows:";
            throw userError(msg, memoryHelp);
          }
          goto exit;

        case 'a':
          /*
            * Add application to simulator wallet.
            * usage:
            *     ./drachma -aa APPLICATION_FILE [APPLICATION_NAME]
            *
            * Remove application to simulator wallet.
            * usage:
            *     ./drachma -ar APPLICATION_FILE [APPLICATION_NAME]
            */

          // Require two switches.
          if (switch_count == 2) {
            const auto& app_file = argv[2];

            const auto& second_switch = switch_choices[1];
            if (second_switch == 'a') {
              std::cout << "Adding new application '" << app_file << "' to the wallet.\n\n";
              valid_use = true;

              if (fileExists(app_file)) {
                if (fileHandler::isValidAppFile(app_file)) {
                  library.addApplication(app_file);

                  std::cout << "Done!\n";
                } else {
                  const auto msg = "The target file '" + std::string(app_file) + "' is not a valid application file.";
                  throw userError(msg);
                }
              } else {
                const auto msg = "The target file '" + std::string(app_file) + "' does not exist.";
                throw userError(msg);
              }

            } else if (second_switch == 'r') {
              std::cout << "Removing application '" << app_file << "' from the wallet.\n\n";
              valid_use = true;

              if (!library.removeApplication(app_file)) {
                const auto msg = "Could not remove application '" + std::string(app_file) + "' from the wallet. Application was not present.";
                throw userError(msg);
              }
            }
          
            if (!valid_use) {
              const auto msg = "Improper switch use. The operations concerning application files are as follows:";
              throw userError(msg, deviceHelp);
            }
            goto exit;
          }

        case 't':
          /*
           * Add application task graph to simulator wallet.
           * usage:
           *     ./drachma -ta TASK_GRAPH_FILE [APPLICATION_NAME]
           *
           * Remove application task graph from simulator wallet.
           * usage:
           *     ./drachma -tr TASK_GRAPH_FILE [APPLICATION_NAME]
           */

          // Require two switches.
          if (switch_count == 2) {
            const auto& tg_file = argv[2]; 

            const auto& second_switch = switch_choices[1];
            if (second_switch == 'a') {
              std::cout << "Adding new task graph file '" << tg_file << "' to the wallet.\n\n";
              valid_use = true;

              if (fileExists(tg_file)) {
                auto trace_reader = fileHandler(
                  tg_file,
                  library.getTraceParamRules(),
                  library.getTraceRegexRules()
                );

                if (trace_reader.isFileValid()) {
                  library.addTraceFile(tg_file);

                  std::cout << "Done!\n";
                } else {
                  const auto msg = "The target file '" + std::string(tg_file) + "' is not a valid task graph file.";
                  throw userError(msg);
                }
              } else {
                const auto msg = "The target file '" + std::string(tg_file) + "' does not exist.";
                throw userError(msg);
              }

            } else if (second_switch == 'r') {
              std::cout << "Removing task graph file '" << tg_file << "' from the wallet.\n\n";
              valid_use = true;

              if (!library.removeTaskGraphFile(tg_file)) {
                const auto msg = "Could not remove task graph file '" + std::string(tg_file) + "' from the wallet; task graph file was not present.";
                throw userError(msg);
              }
            }
          }

          if (!valid_use) {
            const auto msg = "Improper switch use. The operations concerning task graph files are as follows:";
            throw userError(msg, taskGraphHelp);
          }
          goto exit;

        case 'w':
          /*
            * List objects contained in the simulator wallet.
            *
            * usage:
            *     ./drachma -w
            */

          library.printDetails();
          goto exit;

        case 's':
          /*
            * Start simulation.
            *
            * usage:
            *     ./drachma -s MEMORY_ID APPLICATION_ID TASK_GRAPH_ID
            */

          if (argc == 5 || argc == 6) {
            const auto memory_id = atoi(argv[2]);
            const auto application_id = atoi(argv[3]);
            const auto task_graph_id = atoi(argv[4]);

            // Initialize the simulation device.
            device sim_device;

            // Get memory hierarchy, aplication, and task graph.
            auto memory = library.getMemory(memory_id);
            const auto app = library.getApplication(application_id);
            auto task_graph = library.getTaskGraph(task_graph_id);

            // Apply memory hierarchy to device.
            sim_device.associateHierarchy(memory);

            // Prepare device resources needed by the application.
            if (sim_device.prepareAppResources(app)) {
              sim_device.assignTaskGraph(task_graph);

              // If specified, stop the simulation after a certain mount of clock cycles.
              if (argc == 5) {
                sim_device.simulateApplication();
              } else {
                const auto max_cc_count = atoi(argv[5]);
                sim_device.simulateApplication(max_cc_count);
              }

            } else {
              const auto msg = "Chosen application's bitstream library is too large to fit in the selected memory hierarchy.";
              throw userError(msg);
            }

          } else {
            const auto msg = "Improper switch use. The operations concerning starting the simulator are as follows:";
            throw userError(msg, startHelp);
          }
          goto exit;

        // Unrecognized switch
        default:
          const auto msg = "Invalid switch. Here's some help:";
          throw userError(msg, displayHelp);
      }
    }

exit:
  std::cout << std::endl;
  return EXIT_SUCCESS;
}
