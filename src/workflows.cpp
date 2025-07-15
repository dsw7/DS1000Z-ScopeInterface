#include "workflows.hpp"

#include "scope.hpp"
#include "toml.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace {

std::string get_path_to_project_config()
{
    const char *home_dir = std::getenv("HOME");

    if (not home_dir) {
        throw std::runtime_error("Could not determine home directory");
    }

    const std::filesystem::path config_path = std::filesystem::path(home_dir) / ".scope.toml";

    if (not std::filesystem::exists(config_path)) {
        throw std::runtime_error("Configuration file .scope.toml not found in home directory");
    }

    return config_path.string();
}

struct ConfigsMeasureCalSignal {
    float horizontal_position = 0.00;
    float secs_per_div = 0.0005;
    float trigger_level = 1.00;
    float vertical_position = 0.00;
    float volts_per_div = 1.00;
};

void load_config_file(ConfigsMeasureCalSignal &configs)
{
    const std::string config_file = get_path_to_project_config();

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
}

} // namespace

namespace workflows {

void reset_device(const parameters::Parameters &params)
{
    scope::Scope scope_handle(params.host.value(), params.port, params.enable_verbosity);
    scope_handle.handshake();
    scope_handle.reset();
}

void measure_cal_signal(const parameters::Parameters &params)
{
    ConfigsMeasureCalSignal configs;
    load_config_file(configs);

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
