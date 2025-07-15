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

    if (this->secs_per_div < 0) {
        throw std::invalid_argument("Timebase must be greater than 0 seconds");
    }

    if (this->secs_per_div > 50) {
        throw std::invalid_argument("Timebase cannot exceed 50 seconds");
    }
}

void Parameters::set_trigger_level(const std::string &trigger_level)
{
    try {
        this->trigger_level = std::stof(trigger_level);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert trigger level to floating point value: " + (std::string)e.what());
    }

    if (this->trigger_level < -5.00) {
        throw std::invalid_argument("Trigger level cannot be less than -5.00V");
    }

    if (this->trigger_level > 5.00) {
        throw std::invalid_argument("Trigger level cannot exceed 5.00V");
    }
}

void Parameters::set_scale(const std::string &volts_per_div)
{
    try {
        this->volts_per_div = std::stof(volts_per_div);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert scale to floating point value: " + (std::string)e.what());
    }

    if (this->volts_per_div < -0.01) {
        throw std::invalid_argument("Vertical scale cannot be less than 0.01V");
    }

    if (this->volts_per_div > 100) {
        throw std::invalid_argument("Vertical scale cannot exceed 100V");
    }
}

void Parameters::set_vertical_position(const std::string &vertical_position)
{
    try {
        this->vertical_position = std::stof(vertical_position);
    } catch (const std::invalid_argument &e) {
        throw std::invalid_argument("Failed to convert vertical position to floating point value: " + (std::string)e.what());
    }

    if (this->vertical_position < -10.00) {
        throw std::invalid_argument("Vertical position cannot be less than -10.00V");
    }

    if (this->vertical_position > 10.00) {
        throw std::invalid_argument("Vertical position cannot be greater than 10.00V");
    }
}

} // namespace parameters
