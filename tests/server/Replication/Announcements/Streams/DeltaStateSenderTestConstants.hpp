#pragma once

#include <cstddef>
#include <cstdint>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The network ids of the two enemies the tests place, the first of
 * which is the one that moves or vanishes. */
constexpr std::uint32_t MOVING_ENEMY_ID = 1;
constexpr std::uint32_t STILL_ENEMY_ID = 2;

/** @brief Where the enemies start, and where the first one moves to. */
constexpr game::Position FIRST_PLACE{.x = 10.0F, .y = 20.0F};
constexpr game::Position SECOND_PLACE{.x = 30.0F, .y = 40.0F};
constexpr game::Position MOVED_PLACE{.x = 11.0F, .y = 21.0F};

/** @brief Ticks of one world state at the default rate: 60 / 20. */
constexpr std::size_t TICKS_PER_STATE = 3;

/** @brief The ticks run until the first state, and until the second. */
constexpr std::size_t TICKS_TO_THE_FIRST_STATE = 3;

/** @brief Ticks after the first state, at which the state still reaches back
 * to it as far as a delta may (60), and the state after that one (63). */
constexpr std::size_t TICKS_TO_THE_FARTHEST_REFERENCE = 60;
constexpr std::size_t TICKS_BEYOND_THE_FARTHEST_REFERENCE = 63;

/** @brief Enemies that need three datagrams to list in full. */
constexpr std::size_t ENEMIES_FOR_THREE_PARTS = 300;

/** @brief The ports of the two clients. */
constexpr std::uint16_t FIRST_PLAYER_PORT = 4100;
constexpr std::uint16_t SECOND_PLAYER_PORT = 4101;

}  // namespace rtype::server::testing
