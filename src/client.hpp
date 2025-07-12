#pragma once

#include <string>

namespace client {

class TCPConn {
public:
    TCPConn();
    ~TCPConn();

    void establish_connection(const std::string &host, int port);
    void handshake();
    void run();
    void stop();
    void single();
    void set_timebase(float sec);

private:
    int check_for_error_(std::string &errmsg);
    std::string receive_message_();
    void send_message_(const std::string &message);

    int client_fd_;
};

} // namespace client
