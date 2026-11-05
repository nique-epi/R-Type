#pragma once

#include <cstddef>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief Where the orb of the burst tests stands; nothing moves it. */
constexpr game::Position ORB_CENTER{.x = 960.0F, .y = 540.0F};

/** @brief The players told about the shards: the first one connected. */
constexpr std::size_t TOLD_PLAYER = 0;

/** @brief How far apart two lengths may be and still be the same. */
constexpr float SHARD_TOLERANCE_UNITS = 1e-3F;

}  // namespace rtype::server::testing
