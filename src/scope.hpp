#pragma once

#include "tcp_conn.hpp"

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
    Scope(const std::string &host, int port, bool enable_verbosity);
    ~Scope();

    void check_for_error();
    void handshake();
    void reset();
    void run();
    void stop();
    void single();
    void set_timebase(float secs_per_div);
    void set_channel_scale(float volts_per_div);
    void set_rising_edge_trigger(float level);
    void set_horizontal_position(float t);
    void set_vertical_position(float v);
    HorizontalLimits get_horizontal_limits();
    VerticalLimits get_vertical_limits();

private:
    tcp_conn::TCPConn conn_;
};

} // namespace scope
