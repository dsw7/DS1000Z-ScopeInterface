#pragma once

#include <optional>
#include <string>

namespace parameters {
class Parameters {
public:
    std::optional<std::string> host;
    std::string port_s = "5555";
    std::string timebase_s = "0.005";
};
} // namespace parameters
