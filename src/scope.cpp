#include "scope.hpp"

#include <fmt/core.h>
#include <sstream>
#include <stdexcept>

namespace scope {

Scope::Scope(const std::string &host, int port, bool enable_verbosity)
{
    fmt::print("Attempting to connect to {}:{}\n", host, port);
    this->conn_.connect(host, port, enable_verbosity);
}

Scope::~Scope()
{
    fmt::print("Disconnecting from device\n");
    this->conn_.disconnect();
}

void Scope::check_for_error()
{
    this->conn_.send_message(":SYST:ERR?");
    const std::string error = this->conn_.receive_message();

    std::istringstream err_stream(error);
    int code = 0;
    std::string errmsg;

    if (std::getline(err_stream, errmsg, ',')) {
        code = std::stoi(errmsg);

        if (std::getline(err_stream, errmsg)) {
            errmsg.erase(0, errmsg.find_first_not_of(" \t\n\r"));
        }
    }

    if (code != 0) {
        throw std::runtime_error(errmsg);
    }
}

HorizontalLimits Scope::get_horizontal_limits()
{
    this->conn_.send_message(":TIM:MAIN:SCAL?");
    const std::string scale = this->conn_.receive_message();
    this->check_for_error();

    float secs_per_div = 0;

    try {
        secs_per_div = std::stof(scale);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error(e.what());
    }

    HorizontalLimits limits;
    limits.t_max = secs_per_div * 6;
    limits.t_min = secs_per_div * -6;
    return limits;
}

VerticalLimits Scope::get_vertical_limits()
{
    this->conn_.send_message(":CHAN1:SCAL?");
    const std::string scale = this->conn_.receive_message();
    this->check_for_error();

    float volts_per_div = 0;

    try {
        volts_per_div = std::stof(scale);
    } catch (const std::invalid_argument &e) {
        throw std::runtime_error(e.what());
    }

    VerticalLimits limits;
    limits.v_max = volts_per_div * 4;
    limits.v_min = volts_per_div * -4;
    return limits;
}

void Scope::reset()
{
    this->conn_.send_message("*RST");
    this->check_for_error();
}

void Scope::handshake()
{
    this->conn_.send_message("*IDN?");
    fmt::print("Connected to instrument: {}\n", this->conn_.receive_message());
    this->check_for_error();
}

void Scope::run()
{
    this->conn_.send_message(":RUN");
    this->check_for_error();
}

void Scope::stop()
{
    this->conn_.send_message(":STOP");
    this->check_for_error();
}

void Scope::single()
{
    this->conn_.send_message(":SING");
    this->check_for_error();
}

void Scope::set_timebase(float secs_per_div)
{
    this->conn_.send_message(fmt::format(":TIM:MAIN:SCAL {}", secs_per_div));
    this->check_for_error();
}

void Scope::set_channel_scale(float volts_per_div)
{
    this->conn_.send_message(fmt::format(":CHAN1:SCAL {}", volts_per_div));
    this->check_for_error();
}

void Scope::set_rising_edge_trigger(float level)
{
    const VerticalLimits limits = this->get_vertical_limits();

    if (level < limits.v_min or level > limits.v_max) {
        // I/O between machine and scope will crash if we try to set trigger outside of limits
        const std::string errmsg = fmt::format("Trigger outside limits. The vertical limits are {}V and +{}V", limits.v_min, limits.v_max);
        throw std::invalid_argument(errmsg);
    }

    this->conn_.send_message(":TRIG:MODE EDGE");
    this->check_for_error();

    this->conn_.send_message(fmt::format(":TRIG:EDG:LEV {}", level));
    this->check_for_error();

    this->conn_.send_message(":TRIG:EDG:SOUR CHAN1");
    this->check_for_error();

    this->conn_.send_message(":TRIG:EDG:SLOP POS");
    this->check_for_error();
}

void Scope::set_horizontal_position(float t)
{
    const HorizontalLimits limits = this->get_horizontal_limits();

    if (t < limits.t_min or t > limits.t_max) {
        const std::string errmsg = fmt::format("Horizontal position (x) outside limits. The limits are {} s and +{} s", limits.t_min, limits.t_max);
        throw std::invalid_argument(errmsg);
    }

    this->conn_.send_message(fmt::format(":TIM:MAIN:OFFS {}", t));
    this->check_for_error();
}

void Scope::set_vertical_position(float v)
{
    const VerticalLimits limits = this->get_vertical_limits();

    if (v < limits.v_min or v > limits.v_max) {
        const std::string errmsg = fmt::format("Vertical position (y) outside limits. The vertical limits are {}V and +{}V", limits.v_min, limits.v_max);
        throw std::invalid_argument(errmsg);
    }

    this->conn_.send_message(fmt::format(":CHAN1:OFFS {}", v));
    this->check_for_error();
}

} // namespace scope
