#pragma once

#include <cstdint>

namespace rtype::server::testing {

constexpr std::uint32_t DESTROYED_NETWORK_ID = 9;
constexpr std::uint32_t ENEMY_POINTS = 100;

/** @brief The lives the rules of a game give its players, other than the
 * default lives. */
constexpr std::uint8_t RULED_LIVES = 5;

}  // namespace rtype::server::testing
