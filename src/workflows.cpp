#include "workflows.hpp"

#include "client.hpp"

namespace workflows {

void example(const parameters::Parameters &params)
{
    client::TCPConn tcp_conn;
    tcp_conn.establish_connection(params.host.value(), params.port);
    tcp_conn.handshake();
    tcp_conn.run();
    tcp_conn.stop();
}

} // namespace workflows
