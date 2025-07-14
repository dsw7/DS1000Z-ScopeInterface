#include "client.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
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
        std::cout << "DEBUG> Send message: " << message_;
        std::cout << "DEBUG> Bytes sent: " << bytes_sent << '\n';
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
        std::cout << "DEBUG> Received message: " << message << '\n';
        std::cout << "DEBUG> Bytes received: " << bytes_received << '\n';
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
    std::cout << "Attempting to connect to " << host << ":" << port << '\n';

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

void TCPConn::handshake()
{
    std::cout << "Handshaking with device\n";

    this->send_message_(":SYST:LANG?");
    const std::string system_language = this->receive_message_();

    this->check_for_error_();
    std::cout << "Connection successful. Resolved system language: " << system_language << '\n';
}

void TCPConn::clear()
{
    std::cout << "Pressing CLEAR button\n";
    this->send_message_(":CLE");
    this->check_for_error_();
}

void TCPConn::run()
{
    std::cout << "Pressing RUN button\n";
    this->send_message_(":RUN");
    this->check_for_error_();
}

void TCPConn::stop()
{
    std::cout << "Pressing STOP button\n";
    this->send_message_(":STOP");
    this->check_for_error_();
}

void TCPConn::single()
{
    std::cout << "Pressing SINGLE button\n";
    this->send_message_(":SING");
    this->check_for_error_();
}

void TCPConn::set_timebase(float sec)
{
    if (sec < 0) {
        throw std::invalid_argument("Timebase must be greater than 0 seconds");
    }

    if (sec > 50) {
        throw std::invalid_argument("Timebase cannot exceed 50 seconds");
    }

    std::cout << "Setting timebase scale to " << sec << " seconds / division\n";
    this->send_message_(":TIM:MAIN:SCAL " + std::to_string(sec));
    this->check_for_error_();
}

void TCPConn::set_rising_edge_trigger(float level)
{
    if (level < -5.00) {
        throw std::invalid_argument("Trigger level cannot be less than -5.00V");
    }

    if (level > 5.00) {
        throw std::invalid_argument("Trigger level cannot exceed 5.00V");
    }

    std::cout << "Setting rising edge trigger on CH1 with trigger set at " << level << "V\n";
    this->send_message_(":TRIG:MODE EDGE");
    this->check_for_error_();

    this->send_message_(":TRIG:EDG:SOUR CHAN1");
    this->check_for_error_();

    this->send_message_(":TRIG:EDG:SLOP POS");
    this->check_for_error_();

    this->send_message_(":TRIG:EDG:LEV " + std::to_string(level));
    this->check_for_error_();
}

void TCPConn::set_channel_scale(float volts_per_div)
{
    if (volts_per_div < -0.01) {
        throw std::invalid_argument("Vertical scale cannot be less than 0.01V");
    }

    if (volts_per_div > 100) {
        throw std::invalid_argument("Vertical scale cannot exceed 100V");
    }

    this->send_message_(":CHAN1:SCAL " + std::to_string(volts_per_div));
    this->check_for_error_();
}

} // namespace client
