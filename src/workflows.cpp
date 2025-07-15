#include "workflows.hpp"

#include "client.hpp"

#include <fmt/core.h>

namespace workflows {

void example(const parameters::Parameters &params)
{
    fmt::print("Connecting to device\n");

    client::TCPConn tcp_conn(params.enable_verbosity);
    tcp_conn.establish_connection(params.host.value(), params.port);
    tcp_conn.reset();
    tcp_conn.handshake();

    tcp_conn.set_timebase(params.secs_per_div);
    tcp_conn.set_channel_scale(params.volts_per_div);
    tcp_conn.set_rising_edge_trigger(params.trigger_level);
    tcp_conn.set_horizontal_position(params.horizontal_position);
    tcp_conn.set_vertical_position(params.vertical_position);
    tcp_conn.run();

    fmt::print("Disconnecting from device\n");
}

} // namespace workflows
