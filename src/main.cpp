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
  scope [OPTION]... WORKFLOW

Options:
  -h, --help                 Print help information and exit
  -v, --verbose              Enable additional verbosity for debugging purposes
      --host                 Oscilloscope host (option is mandatory)
      --port                 Oscilloscope port (default is 5555)

Display:
  -t, --timebase             Set horizontal scale (seconds / horizontal division)
  -s, --scale                Set vertical scale (volts / vertical division)
  -x, --horizontal-position  Set channel vertical position (in volts)
  -y, --vertical-position    Set channel vertical position (in volts)

Triggering:
  -l, --trigger-level        Set trigger level (in volts)

Workflows:
  default  Configure scope to measure built in 1KHz calibration wave
  reset    Send *RST command. Restores instrument to the default state
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
            { "host", required_argument, 0, 0 },
            { "port", required_argument, 0, 0 },
            { "timebase", required_argument, 0, 't' },
            { "trigger-level", required_argument, 0, 'l' },
            { "scale", required_argument, 0, 's' },
            { "horizontal-position", required_argument, 0, 'x' },
            { "vertical-position", required_argument, 0, 'y' },
            { 0, 0, 0, 0 },
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hvt:l:s:x:y:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 0:
                if (strcmp(long_options[option_index].name, "host") == 0) {
                    params.host = optarg;
                } else if (strcmp(long_options[option_index].name, "port") == 0) {
                    params.set_port(optarg);
                }
                break;
            case 'h':
                print_help_messages();
                exit(EXIT_SUCCESS);
            case 'v':
                params.enable_verbosity = true;
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
            case 'x':
                params.set_horizontal_position(optarg);
                break;
            case 'y':
                params.set_vertical_position(optarg);
                break;
            default:
                print_help_messages();
                exit(EXIT_FAILURE);
        }
    };

    if (not params.host) {
        throw std::invalid_argument("Host is unspecified. Set with --host option");
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("scope", argv[i]) != 0) {
            params.workflow = argv[i];
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

    bool failed = false;
    std::string errmsg;

    try {
        if (params.workflow == "reset") {
            workflows::reset_device(params);
        } else {
            workflows::run_default(params);
        }
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
