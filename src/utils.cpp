#include "utils.hpp"

#include <filesystem>
#include <stdexcept>

namespace utils {

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

} // namespace utils
