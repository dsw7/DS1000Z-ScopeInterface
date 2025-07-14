#pragma once

#include <optional>
#include <string>

namespace parameters {

class Parameters {
public:
    bool enable_verbosity = false;
    float secs_per_div = 0.005;
    float trigger_level = 2.00;
    int port = 5555;
    std::optional<std::string> host;
    std::string port_s = "5555";
    std::string secs_per_div_s = "0.005";
    std::string trigger_level_s = "2.00";

    void run_conversions();

private:
    void port_s_to_port_();
    void timebase_s_to_timebase_();
    void trigger_level_s_to_trigger_level_();
};

} // namespace parameters
