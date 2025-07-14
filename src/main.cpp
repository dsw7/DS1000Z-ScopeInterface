#include "parameters.hpp"
#include "workflows.hpp"

#include <cstring>
#include <fmt/core.h>
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
  -h, --help             Print help information and exit
  -v, --verbose          Enable additional verbosity for debugging purposes
  -p, --port             Oscilloscope port (default is 5555)
  -t, --timebase         Set horizontal scale (seconds / horizontal division)
  -l, --trigger-level    Set trigger level (in volts)
  -s, --scale            Set vertical scale (volts / vertical division)
  -y, --vertical-offset  Set channel vertical position (in volts)
)";

    fmt::print("-- DS1000Z-ScopeInterface | v{}\n", PROJECT_VERSION);
    fmt::print("{}\n", messages);
}

parameters::Parameters read_cli(int argc, char **argv)
{
    parameters::Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "verbose", no_argument, 0, 'v' },
            { "port", required_argument, 0, 'p' },
            { "timebase", required_argument, 0, 't' },
            { "trigger-level", required_argument, 0, 'l' },
            { "scale", required_argument, 0, 's' },
            { "vertical-offset", required_argument, 0, 'y' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hvp:t:l:s:y:", long_options, &option_index);

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
            case 'p':
                params.set_port(optarg);
                break;
            case 't':
                params.set_timebase(optarg);
                break;
            case 'l':
                params.set_trigger_level(optarg);
                break;
            case 's':
                params.set_scale(optarg);
                break;
            case 'y':
                params.set_vertical_offset(optarg);
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

    return params;
}

int main(int argc, char **argv)
{
    parameters::Parameters params;

    try {
        params = read_cli(argc, argv);
    } catch (const std::invalid_argument &e) {
        std::cerr << fmt::format("{}\n", e.what());
        return 1;
    }

    if (not params.host) {
        std::cerr << "Host argument is not set!\n";
        return 1;
    }

    bool failed = false;
    std::string errmsg;

    try {
        workflows::example(params);
    } catch (const std::runtime_error &e) {
        failed = true;
        errmsg = e.what();
    } catch (const std::invalid_argument &e) {
        failed = true;
        errmsg = e.what();
    }

    if (failed) {
        std::cerr << fmt::format("An error occurred: {}\n", errmsg);
        return 1;
    }

    return 0;
}
