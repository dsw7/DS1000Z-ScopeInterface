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
    void set_timebase(float secs_per_div);
    void set_rising_edge_trigger(float level);
    void set_channel_scale(float volts_per_div);
    float get_channel_scale();
    void set_channel_vertical_position(float offset_in_volts);

private:
    void check_for_error_();
    std::string receive_message_();
    void send_message_(const std::string &message);

    bool is_connected_ = false;
    bool verbose_ = false;
    int client_fd_;
};

} // namespace client
