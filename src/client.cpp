#include "client.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace {

class TCPConn {
public:
    TCPConn();
    ~TCPConn();
    void establish_connection(const std::string &host, int port);
    void send_message(const std::string &message);

private:
    int client_fd_;
};

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

void TCPConn::send_message(const std::string &message)
{
    int bytes_sent = send(this->client_fd_, message.c_str(), message.size(), 0);

    if (bytes_sent == -1) {
        throw std::runtime_error(std::strerror(errno));
    }

    std::cout << bytes_sent << '\n';
}

} // namespace

namespace client {

void send_message(const parameters::Parameters &params)
{
    TCPConn tcp_conn;
    tcp_conn.establish_connection(params.host.value(), params.port);
    tcp_conn.send_message(":TIMebase:MAIN:SCALe 0.005\r\n");
}

} // namespace client
