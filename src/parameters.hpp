#pragma once

#include <optional>
#include <string>

namespace parameters {

struct Parameters {
    // general
    bool enable_verbosity = false;
    int port = 5555;
    std::optional<std::string> host;

    // trigger
    float trigger_level = 1.00;

    // display
    float horizontal_position = 0.00;
    float secs_per_div = 0.0005;
    float vertical_position = 0.00;
    float volts_per_div = 1.00;

    void set_port(const std::string &port);
    void set_timebase(const std::string &secs_per_div);
    void set_trigger_level(const std::string &trigger_level);
    void set_scale(const std::string &volts_per_div);
    void set_horizontal_position(const std::string &horizontal_position);
    void set_vertical_position(const std::string &vertical_position);
};

} // namespace parameters
