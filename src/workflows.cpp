#include "workflows.hpp"

#include "configs.hpp"
#include "scope.hpp"

namespace workflows {

void reset_device(const parameters::Parameters &params)
{
    scope::Scope scope_handle(params.host.value(), params.port, params.enable_verbosity);
    scope_handle.handshake();
    scope_handle.reset();
}

void run_default(const parameters::Parameters &params)
{
    const configs::ConfigsDefaultSection configs = configs::load_default_section();

    scope::Scope scope_handle(params.host.value(), params.port, params.enable_verbosity);
    scope_handle.handshake();
    scope_handle.set_timebase(configs.secs_per_div);
    scope_handle.set_channel_scale(configs.volts_per_div);
    scope_handle.set_rising_edge_trigger(configs.trigger_level);
    scope_handle.set_horizontal_position(configs.horizontal_position);
    scope_handle.set_vertical_position(configs.vertical_position);
    scope_handle.run();
}

} // namespace workflows
