#include "workflows.hpp"

#include "client.hpp"

namespace workflows {

void example(const parameters::Parameters &params)
{
    client::TCPConn tcp_conn(params.enable_verbosity);
    tcp_conn.establish_connection(params.host.value(), params.port);
    tcp_conn.handshake();
    tcp_conn.set_timebase(params.timebase);
    tcp_conn.single();
}

} // namespace workflows
