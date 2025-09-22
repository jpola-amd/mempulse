#pragma once

#include <stdexcept>

#define check_drm(err, msg) if (err != 0) throw std::runtime_error(msg);
