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
}

std::string TCPConn::receive_message_()
{
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

    return message;
}

int TCPConn::check_for_error_(std::string &errmsg)
{
    this->send_message_(":SYST:ERR?");
    const std::string error = this->receive_message_();

    std::istringstream err_stream(error);
    int code = 0;

    if (std::getline(err_stream, errmsg, ',')) {
        code = std::stoi(errmsg);

        if (std::getline(err_stream, errmsg)) {
            errmsg.erase(0, errmsg.find_first_not_of(" \t\n\r"));
        }
    }

    return code;
}

// ----------------------------------------------------------------------------------------------------------
// public
// ----------------------------------------------------------------------------------------------------------

TCPConn::TCPConn()
{
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
}

void TCPConn::establish_connection(const std::string &host, int port)
{
    std::cout << "Attempting to connect to host: " << host << '\n';
    std::cout << "On port: " << port << '\n';

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
}

void TCPConn::handshake()
{
    std::cout << "Handshaking with device\n";

    this->send_message_(":SYST:LANG?");
    const std::string system_language = this->receive_message_();

    std::string errmsg;
    if (this->check_for_error_(errmsg) < 0) {
        throw std::runtime_error(errmsg);
    }

    std::cout << "Connection successful. Resolved system language: " << system_language << '\n';
}

void TCPConn::run()
{
    std::cout << "Pressing RUN button\n";
    this->send_message_(":RUN");
}

void TCPConn::stop()
{
    std::cout << "Pressing STOP button\n";
    this->send_message_(":STOP");
}

} // namespace client
