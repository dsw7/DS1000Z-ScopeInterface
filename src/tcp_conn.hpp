#pragma once

#include <string>

namespace tcp_conn {

class TCPConn {
public:
    void connect(const std::string &host, int port, bool enable_verbosity = false);
    void disconnect();
    void send_message(const std::string &message);
    std::string receive_message();

private:
    bool is_connected_ = false;
    bool verbose_ = false;
    int sockfd_ = 0;
};

} // namespace tcp_conn
