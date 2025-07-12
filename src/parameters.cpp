#include "parameters.hpp"

#include <stdexcept>

namespace parameters {

void Parameters::port_s_to_port_()
{
    try {
        this->port = std::stoi(this->port_s);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Failed to convert port to integer value: " + (std::string)e.what());
    }
}

void Parameters::timebase_s_to_timebase_()
{
    try {
        this->timebase = std::stof(this->timebase_s);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Failed to convert timebase to floating point value: " + (std::string)e.what());
    }
}

void Parameters::run_conversions()
{
    this->port_s_to_port_();
    this->timebase_s_to_timebase_();
}

} // namespace parameters
