#include "parameters.hpp"

#include <stdexcept>

namespace parameters {

void Parameters::set_port(const std::string &port)
{
    try {
        this->port = std::stoi(port);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert port to integer value: " + (std::string)e.what());
    }
}

} // namespace parameters
