#include "workflows.hpp"

#include "scope.hpp"
#include "toml.hpp"
#include "utils.hpp"

#include <stdexcept>

namespace {

struct ConfigsDefaultSection {
    float horizontal_position = 0.00;
    float secs_per_div = 0.0005;
    float trigger_level = 1.00;
    float vertical_position = 0.00;
    float volts_per_div = 1.00;
};

ConfigsDefaultSection load_default_section()
{
    const std::string config_file = utils::get_path_to_project_config();
    ConfigsDefaultSection configs;

    try {
        const toml::table config = toml::parse_file(config_file);
        configs.horizontal_position = config["workflows"]["default"]["horizontal_position"].value_or(0.00);
        configs.secs_per_div = config["workflows"]["default"]["timebase"].value_or(0.0005);
        configs.trigger_level = config["workflows"]["default"]["trigger_level"].value_or(1.00);
        configs.vertical_position = config["workflows"]["default"]["vertical_position"].value_or(0.00);
        configs.volts_per_div = config["workflows"]["default"]["scale"].value_or(1.00);
    } catch (const toml::parse_error &e) {
        throw std::runtime_error(e.what());
    }

    return configs;
}

} // namespace

namespace workflows {

void reset_device(const parameters::Parameters &params)
{
    scope::Scope scope_handle(params.host.value(), params.port, params.enable_verbosity);
    scope_handle.handshake();
    scope_handle.reset();
}

void run_default(const parameters::Parameters &params)
{
    const ConfigsDefaultSection configs = load_default_section();

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
