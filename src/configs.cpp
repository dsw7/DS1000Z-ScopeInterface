#include "configs.hpp"

#include "toml.hpp"

#include <filesystem>
#include <fmt/core.h>
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

} // namespace

namespace configs {

void ConfigsDefaultSection::run_validation()
{
    if (this->secs_per_div <= 0 or this->secs_per_div > 50) {
        throw std::invalid_argument("Timebase must be > 0 and <= 50 seconds");
    }

    if (this->trigger_level < -5.00 or this->trigger_level > 5.00) {
        throw std::invalid_argument("Trigger level must be between -5.00V and 5.00V");
    }

    if (this->volts_per_div < -0.01 or this->volts_per_div > 100) {
        throw std::invalid_argument("Vertical scale must be between 0.01V and 100V");
    }

    if (this->vertical_position < -10.00 or this->vertical_position > 10.00) {
        throw std::invalid_argument("Vertical position must be between -10.00V and 10.00V");
    }
}

ConfigsDefaultSection load_default_section()
{
    const std::string config_file = get_path_to_project_config();
    ConfigsDefaultSection configs;

    try {
        const toml::table config = toml::parse_file(config_file);
        const auto section = config["workflows.default"];

        configs.horizontal_position = section["horizontal_position"].value_or(0.00);
        configs.secs_per_div = section["timebase"].value_or(0.0005);
        configs.trigger_level = section["trigger_level"].value_or(1.00);
        configs.vertical_position = section["vertical_position"].value_or(0.00);
        configs.volts_per_div = section["scale"].value_or(1.00);
    } catch (const toml::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse TOML: {}", e.what()));
    }

    return configs;
}

} // namespace configs
