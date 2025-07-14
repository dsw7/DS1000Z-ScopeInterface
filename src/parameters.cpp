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

void Parameters::set_timebase(const std::string &secs_per_div)
{
    try {
        this->secs_per_div = std::stof(secs_per_div);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert timebase to floating point value: " + (std::string)e.what());
    }
}

void Parameters::set_trigger_level(const std::string &trigger_level)
{
    try {
        this->trigger_level = std::stof(trigger_level);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert trigger level to floating point value: " + (std::string)e.what());
    }
}

void Parameters::set_scale(const std::string &volts_per_div)
{
    try {
        this->volts_per_div = std::stof(volts_per_div);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert scale to floating point value: " + (std::string)e.what());
    }
}

} // namespace parameters
