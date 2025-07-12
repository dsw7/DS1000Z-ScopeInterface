#include "client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace {

class TCPConn {
public:
    TCPConn();
    ~TCPConn();

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

} // namespace

namespace client {

void send_message(const parameters::Parameters &params)
{
    TCPConn tcp_conn;

    std::cout << params.port << '\n';
    // The TCPConn destructor will automatically close the socket when tcp_conn goes out of scope
}

} // namespace client
