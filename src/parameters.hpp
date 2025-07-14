#pragma once

#include <optional>
#include <string>

namespace parameters {

struct Parameters {
    bool enable_verbosity = false;
    float secs_per_div = 0.005;
    float trigger_level = 2.00;
    float volts_per_div = 1.00;
    int port = 5555;

    std::optional<std::string> host;

    void set_port(const std::string &port);
    void set_timebase(const std::string &secs_per_div);
    void set_trigger_level(const std::string &trigger_level);
    void set_scale(const std::string &volts_per_div);
};

} // namespace parameters
