#pragma once

#include <string>

namespace scope {

struct HorizontalLimits {
    float t_min = 0.00;
    float t_max = 0.00;
};

struct VerticalLimits {
    float v_min = 0.00;
    float v_max = 0.00;
};

class Scope {
public:
    Scope(bool verbose = false);
    ~Scope();

    void establish_connection(const std::string &host, int port);
    void reset();
    void handshake();
    void run();
    void stop();
    void single();
    void set_timebase(float secs_per_div);
    void set_channel_scale(float volts_per_div);
    void set_rising_edge_trigger(float level);
    void set_horizontal_position(float t);
    void set_vertical_position(float v);

private:
    void check_for_error_();
    std::string receive_message_();
    void send_message_(const std::string &message);

    HorizontalLimits get_horizontal_limits_();
    VerticalLimits get_vertical_limits_();

    bool is_connected_ = false;
    bool verbose_ = false;
    int sockfd_ = 0;
};

} // namespace scope
