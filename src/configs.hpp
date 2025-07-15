#pragma once

#include <string>

namespace configs {

struct ConfigsDefaultSection {
    float horizontal_position = 0.00;
    float secs_per_div = 0.0005;
    float trigger_level = 1.00;
    float vertical_position = 0.00;
    float volts_per_div = 1.00;

    void run_validation();
};

ConfigsDefaultSection load_default_section();
} // namespace configs
