#include "tcp_conn.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fmt/core.h>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace tcp_conn {

void TCPConn::connect(const std::string &host, int port, bool enable_verbosity)
{
    this->verbose_ = enable_verbosity;
    this->sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (this->sockfd_ == -1) {
        throw std::runtime_error(std::strerror(errno));
    }

    fmt::print("Attempting to connect to {}:{}\n", host, port);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &server_address.sin_addr) <= 0) {
        throw std::runtime_error("Invalid address or address not supported");
    }

    if (::connect(this->sockfd_, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        close(this->sockfd_);
        throw std::runtime_error("Connection failed");
    }

    this->is_connected_ = true;
}

void TCPConn::disconnect()
{
    if (this->sockfd_ != -1) {
        close(this->sockfd_);
    }

    this->is_connected_ = false;
}

void TCPConn::send_message(const std::string &message)
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

    int bytes_sent = send(this->sockfd_, message_.c_str(), message_.size(), 0);

    if (bytes_sent == -1) {
        throw std::runtime_error(std::strerror(errno));
    }

    if (this->verbose_) {
        fmt::print("Sent message: {} ({} bytes)\n", message_, bytes_sent);
    }
}

std::string TCPConn::receive_message()
{
    if (not this->is_connected_) {
        throw std::logic_error("Cannot receive message. Socket file descriptor is not connected to any address");
    }

    char buffer[1024];
    int bytes_received = recv(this->sockfd_, buffer, sizeof(buffer) - 1, 0);

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

} // namespace tcp_conn
