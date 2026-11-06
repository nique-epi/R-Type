#pragma once

#include <cstddef>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief A second of frames of SIMULATION_TICK_DURATION. */
constexpr std::size_t FRAMES_IN_A_SECOND = 60;

/** @brief The inputs a client sends in a second of game, as the tool
 * requires of each bot. */
constexpr std::size_t INPUTS_IN_A_SECOND = 60;

/** @brief A frame twice as short as a tick, and a second of them. */
constexpr engine::Duration HALF_A_TICK = engine::SIMULATION_TICK_DURATION / 2;
constexpr std::size_t HALF_TICKS_IN_A_SECOND = 120;

/** @brief The player of the bot, another player of its game, the entity of
 * the ship a world state gives the bot, and the newest input of the bot it
 * says the server applied. */
constexpr std::uint8_t BOT_PLAYER_NUMBER = 2;
constexpr std::uint8_t OTHER_PLAYER_NUMBER = 3;
constexpr std::uint32_t SHIP_ENTITY_ID = 40;
constexpr std::uint32_t ACKNOWLEDGED_INPUT = 7;

}  // namespace rtype::tools::testing
