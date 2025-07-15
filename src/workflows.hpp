#pragma once

#include "parameters.hpp"

namespace workflows {
void reset_device(const parameters::Parameters &params);
void measure_cal_signal(const parameters::Parameters &params);
} // namespace workflows
