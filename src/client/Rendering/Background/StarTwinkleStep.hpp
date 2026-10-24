#pragma once

#include <chrono>
#include <cstdint>
#include <ratio>

namespace rtype::client {

/** @brief Twinkling time runs in steps of a third of a second. */
using StarTwinkleStep = std::chrono::duration<std::int64_t, std::ratio<1, 3>>;

}  // namespace rtype::client
