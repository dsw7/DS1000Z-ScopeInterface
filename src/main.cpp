#include "parameters.hpp"
#include "workflows.hpp"

#include <cstring>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>

void print_help_messages()
{
    const std::string messages = R"(-- Copyright (C) 2025 by David Weber

Automation for working with Rigol DS1000Z Series of oscilloscopes

Usage:
  scope [OPTION]
  scope [OPTION]... HOST

Options:
  -h, --help      Print help information and exit
  -t, --timebase  Set timebase scale (units in seconds)
  -p, --port      Oscilloscope port (default is 5555)
  -v, --verbose   Enable additional verbosity for debugging purposes
)";

    std::cout << "-- DS1000Z-ScopeInterface | v" << PROJECT_VERSION << '\n';
    std::cout << messages << '\n';
}

int main(int argc, char **argv)
{
    parameters::Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "verbose", no_argument, 0, 'v' },
            { "timebase", required_argument, 0, 't' },
            { "port", required_argument, 0, 'p' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hvt:p:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 'v':
                params.enable_verbosity = true;
                break;
            case 't':
                params.timebase_s = optarg;
                break;
            case 'p':
                params.port_s = optarg;
                break;
            default:
                print_help_messages();
                exit(EXIT_FAILURE);
        }
    };

    for (int i = optind; i < argc; i++) {
        if (strcmp("scope", argv[i]) != 0) {
            params.host = argv[i];
            break;
        }
    }

    if (not params.host) {
        std::cerr << "Host argument is not set!\n";
        return 1;
    }

    bool failed = false;
    std::string errmsg;

    try {
        params.run_conversions();
        workflows::example(params);
    } catch (const std::runtime_error &e) {
        failed = true;
        errmsg = e.what();
    } catch (const std::invalid_argument &e) {
        failed = true;
        errmsg = e.what();
    }

    if (failed) {
        std::cerr << "An error occurred: " << errmsg << '\n';
        return 1;
    }

    return 0;
}
