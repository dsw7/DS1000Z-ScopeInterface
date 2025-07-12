#pragma once

#include <optional>
#include <string>

namespace parameters {

class Parameters {
public:
    float timebase = 0.005;
    int port = 5555;
    std::optional<std::string> host;
    std::string port_s = "5555";
    std::string timebase_s = "0.005";

    void run_conversions();

private:
    void port_s_to_port_();
    void timebase_s_to_timebase_();
};

} // namespace parameters
