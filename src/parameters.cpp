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
        this->secs_per_div = std::stof(this->secs_per_div_s);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Failed to convert timebase to floating point value: " + (std::string)e.what());
    }
}

void Parameters::trigger_level_s_to_trigger_level_()
{
    try {
        this->trigger_level = std::stof(this->trigger_level_s);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error("Failed to convert trigger level to floating point value: " + (std::string)e.what());
    }
}

void Parameters::run_conversions()
{
    this->port_s_to_port_();
    this->timebase_s_to_timebase_();
    this->trigger_level_s_to_trigger_level_();
}

} // namespace parameters
