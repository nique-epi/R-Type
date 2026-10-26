#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The lives a player starts a game with. */
constexpr std::uint8_t THREE_LIVES = 3;

/** @brief The lives of a player whose next hit eliminates it. */
constexpr std::uint8_t ONE_LIFE = 1;

/** @brief One second at 60 simulation ticks per second: how long a player
 * whose ship was destroyed waits for a new one. */
constexpr std::size_t ONE_SECOND_IN_TICKS = 60;

/** @brief Two seconds at 60 simulation ticks per second: how long a ship that
 * came back after a hit cannot be hit. */
constexpr std::size_t TWO_SECONDS_IN_TICKS = 120;

}  // namespace rtype::server::testing
