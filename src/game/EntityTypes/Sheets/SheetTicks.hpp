#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::game {

/** @returns The whole simulation ticks nearest to @p duration: 333 ms are 20
 * ticks at 60 per second, 1500 ms are 90, 0 ms are 0. */
[[nodiscard]] std::size_t nearestTicksIn(engine::Duration duration);

}  // namespace rtype::game
