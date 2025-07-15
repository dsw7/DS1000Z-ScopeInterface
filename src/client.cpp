#include "client.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fmt/core.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace client {

// ----------------------------------------------------------------------------------------------------------
// private
// ----------------------------------------------------------------------------------------------------------

void TCPConn::send_message_(const std::string &message)
{
    if (not this->is_connected_) {
        throw std::logic_error("Cannot send message. Socket file descriptor is not connected to any address");
    }

    if (message.empty()) {
        throw std::runtime_error("Cannot send empty message");
    }

    std::string message_ = message;

    if (message_.back() != '\n') {
        message_.push_back('\n');
    }

    int bytes_sent = send(this->client_fd_, message_.c_str(), message_.size(), 0);

    if (bytes_sent == -1) {
        throw std::runtime_error(std::strerror(errno));
    }

    if (this->verbose_) {
        fmt::print("Sent message: {} ({} bytes)\n", message_, bytes_sent);
    }
}

std::string TCPConn::receive_message_()
{
    if (not this->is_connected_) {
        throw std::logic_error("Cannot receive message. Socket file descriptor is not connected to any address");
    }

    char buffer[1024];
    int bytes_received = recv(this->client_fd_, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received == -1) {
        throw std::runtime_error(std::strerror(errno));
    }

    buffer[bytes_received] = '\0';
    std::string message(buffer);

    if (not message.empty()) {
        if (message.back() == '\n') {
            message.pop_back();
        }
    }

    if (this->verbose_) {
        fmt::print("Received message: {}\n ({} bytes)\n", message, bytes_received);
    }

    return message;
}

void TCPConn::check_for_error_()
{
    this->send_message_(":SYST:ERR?");
    const std::string error = this->receive_message_();

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

HorizontalLimits TCPConn::get_horizontal_limits_()
{
    this->send_message_(":TIM:MAIN:SCAL?");
    const std::string scale = this->receive_message_();
    this->check_for_error_();

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

VerticalLimits TCPConn::get_vertical_limits_()
{
    this->send_message_(":CHAN1:SCAL?");
    const std::string scale = this->receive_message_();
    this->check_for_error_();

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

// ----------------------------------------------------------------------------------------------------------
// public
// ----------------------------------------------------------------------------------------------------------

TCPConn::TCPConn(bool verbose)
{
    if (verbose) {
        this->verbose_ = true;
    }

    this->client_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (this->client_fd_ == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
}

TCPConn::~TCPConn()
{
    if (this->client_fd_ != -1) {
        close(this->client_fd_);
    }

    this->is_connected_ = false;
}

void TCPConn::establish_connection(const std::string &host, int port)
{
    fmt::print("Attempting to connect to {}:{}\n", host, port);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &server_address.sin_addr) <= 0) {
        throw std::runtime_error("Invalid address or address not supported");
    }

    if (connect(this->client_fd_, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        close(this->client_fd_);
        throw std::runtime_error("Connection failed");
    }

    this->is_connected_ = true;
}

void TCPConn::reset()
{
    this->send_message_("*RST");
    this->check_for_error_();
}

void TCPConn::handshake()
{
    this->send_message_("*IDN?");
    fmt::print("Connected to instrument: {}\n", this->receive_message_());
    this->check_for_error_();
}

void TCPConn::run()
{
    this->send_message_(":RUN");
    this->check_for_error_();
}

void TCPConn::stop()
{
    this->send_message_(":STOP");
    this->check_for_error_();
}

void TCPConn::single()
{
    this->send_message_(":SING");
    this->check_for_error_();
}

void TCPConn::set_timebase(float secs_per_div)
{
    this->send_message_(fmt::format(":TIM:MAIN:SCAL {}", secs_per_div));
    this->check_for_error_();
}

void TCPConn::set_channel_scale(float volts_per_div)
{
    this->send_message_(fmt::format(":CHAN1:SCAL {}", volts_per_div));
    this->check_for_error_();
}

void TCPConn::set_rising_edge_trigger(float level)
{
    const VerticalLimits limits = this->get_vertical_limits_();

    if (level < limits.v_min or level > limits.v_max) {
        // I/O between machine and scope will crash if we try to set trigger outside of limits
        const std::string errmsg = fmt::format("Trigger outside limits. The vertical limits are {}V and +{}V", limits.v_min, limits.v_max);
        throw std::invalid_argument(errmsg);
    }

    this->send_message_(":TRIG:MODE EDGE");
    this->check_for_error_();

    this->send_message_(fmt::format(":TRIG:EDG:LEV {}", level));
    this->check_for_error_();

    this->send_message_(":TRIG:EDG:SOUR CHAN1");
    this->check_for_error_();

    this->send_message_(":TRIG:EDG:SLOP POS");
    this->check_for_error_();
}

void TCPConn::set_horizontal_position(float t)
{
    const HorizontalLimits limits = this->get_horizontal_limits_();

    if (t < limits.t_min or t > limits.t_max) {
        const std::string errmsg = fmt::format("Horizontal position (x) outside limits. The limits are {} s and +{} s", limits.t_min, limits.t_max);
        throw std::invalid_argument(errmsg);
    }

    this->send_message_(fmt::format(":TIM:MAIN:OFFS {}", t));
    this->check_for_error_();
}

void TCPConn::set_vertical_position(float v)
{
    const VerticalLimits limits = this->get_vertical_limits_();

    if (v < limits.v_min or v > limits.v_max) {
        const std::string errmsg = fmt::format("Vertical position (y) outside limits. The vertical limits are {}V and +{}V", limits.v_min, limits.v_max);
        throw std::invalid_argument(errmsg);
    }

    this->send_message_(fmt::format(":CHAN1:OFFS {}", v));
    this->check_for_error_();
}

} // namespace client
