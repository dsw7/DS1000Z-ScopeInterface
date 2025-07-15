#pragma once

#include <optional>
#include <string>

namespace parameters {

struct Parameters {
    bool enable_verbosity = false;
    int port = 5555;
    std::optional<std::string> host;
    std::string workflow = "default";

    void set_port(const std::string &port);
};

} // namespace parameters
