#pragma once

#include <string>

namespace client {

struct ScreenLimits {
    float v_min = 0.00;
    float v_max = 0.00;
};

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
    void set_channel_scale(float volts_per_div);
    ScreenLimits get_channel_scale();
    void set_rising_edge_trigger(float level);
    void set_channel_vertical_position(float offset_in_volts);
    void set_horizontal_position(float position_in_secs);

private:
    void check_for_error_();
    std::string receive_message_();
    void send_message_(const std::string &message);

    bool is_connected_ = false;
    bool verbose_ = false;
    int client_fd_;
};

} // namespace client
