#include "parameters.hpp"

#include <getopt.h>
#include <iostream>
#include <string>

void print_help_messages()
{
    const std::string messages = R"(-- Copyright (C) 2025 by David Weber

Automation for working with Rigol DS1000Z Series of oscilloscopes

Usage:
  scope [OPTION]
  scope [OPTION]... HOST PORT

Options:
  -h, --help      Print help information and exit
  -t, --timebase  Set timebase scale
  -p, --port      Oscilloscope port (default is 5555)
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
            { "timebase", no_argument, 0, 't' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "ht:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 't':
                params.timebase_s = optarg;
                break;
            default:
                print_help_messages();
                exit(EXIT_FAILURE);
        }
    };

    std::cout << params.timebase_s << '\n';

    return 0;
}
