#include "workflows.hpp"

#include "client.hpp"

#include <iostream>

namespace workflows {

void example(const parameters::Parameters &params)
{
    std::cout << "Connecting to device\n";
    client::TCPConn tcp_conn(params.enable_verbosity);
    tcp_conn.establish_connection(params.host.value(), params.port);
    tcp_conn.handshake();

    tcp_conn.set_timebase(params.secs_per_div);
    tcp_conn.set_channel_scale(params.volts_per_div);
    tcp_conn.set_rising_edge_trigger(params.trigger_level);
    tcp_conn.set_channel_vertical_position(params.vertical_offset);
    tcp_conn.run();

    std::cout << "Disconnecting from device\n";
}

} // namespace workflows
