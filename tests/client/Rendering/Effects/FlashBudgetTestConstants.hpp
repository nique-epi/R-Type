#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief How long the tests lend the pixels for. */
constexpr rtype::engine::Duration LOAN_TIME = std::chrono::milliseconds(100);
