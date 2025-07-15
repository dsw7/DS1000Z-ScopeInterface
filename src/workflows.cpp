#include "workflows.hpp"

#include "scope.hpp"

namespace workflows {

void reset_device(const parameters::Parameters &params)
{
    scope::Scope scope_handle(params.host.value(), params.port, params.enable_verbosity);
    scope_handle.handshake();
    scope_handle.reset();
}

void measure_cal_signal(const parameters::Parameters &params)
{
    scope::Scope scope_handle(params.host.value(), params.port, params.enable_verbosity);
    scope_handle.handshake();
    scope_handle.set_timebase(params.secs_per_div);
    scope_handle.set_channel_scale(params.volts_per_div);
    scope_handle.set_rising_edge_trigger(params.trigger_level);
    scope_handle.set_horizontal_position(params.horizontal_position);
    scope_handle.set_vertical_position(params.vertical_position);
    scope_handle.run();
}

} // namespace workflows
