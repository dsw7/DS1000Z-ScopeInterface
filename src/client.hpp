#pragma once

#include <string>

namespace client {

class TCPConn {
public:
    TCPConn(bool verbose = false);
    ~TCPConn();

    void establish_connection(const std::string &host, int port);
    void handshake();
    void clear();
    void run();
    void stop();
    void single();
    void set_timebase(float sec);
    void set_rising_edge_trigger(float level);

private:
    void check_for_error_();
    std::string receive_message_();
    void send_message_(const std::string &message);

    bool is_connected_ = false;
    bool verbose_ = false;
    int client_fd_;
};

} // namespace client
