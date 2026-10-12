#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Kinds of sky events, in the order of SKY_EVENT_WEIGHTS. */
enum class SkyEventKind : std::uint8_t { ShootingStar, Flare, Asteroid, Comet };

}  // namespace rtype::client
