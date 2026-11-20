#pragma once

#include <cstddef>
#include <cstdint>
#include "Bounds.hpp"
#include "PlayBand.hpp"
#include "Position.hpp"
#include "PredictionConstants.hpp"
#include "ShipControlConstants.hpp"
#include "ShipDirections.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

constexpr game::Position START{.x = 400.0F, .y = 300.0F};

/** @brief The band of the Pit, rows 192 to 848, a ship near its floor, and
 * the row the center of a ship stops on against it: half a ship (32 units)
 * above it. */
constexpr game::PlayBand PIT_BAND{.top = 192.0F, .bottom = 848.0F};
constexpr game::Position NEAR_THE_PIT_FLOOR{.x = 400.0F, .y = 760.0F};
constexpr float PIT_FLOOR_STOP_Y = 816.0F;

/** @brief A band whose ceiling, row 400, lies below START, and the row the
 * center of a ship stops on under it. */
constexpr game::PlayBand BAND_BELOW_THE_START{.top = 400.0F, .bottom = 900.0F};
constexpr float BELOW_THE_START_STOP_Y = 432.0F;

constexpr game::ShipDirections HOLDING_RIGHT{.right = true};
constexpr game::ShipDirections HOLDING_DOWN{.down = true};
constexpr game::ShipDirections HOLDING_NOTHING{};

constexpr std::uint32_t FIRST_COMMAND = 1;
constexpr std::uint32_t SECOND_COMMAND = 2;
constexpr std::uint32_t THIRD_COMMAND = 3;
constexpr std::uint32_t OLDER_THAN_ANY_KEPT_COMMAND = 0;
constexpr std::uint32_t COMMAND_AFTER_THE_DOUBLE_TAP_WINDOW =
    FIRST_COMMAND + game::DOUBLE_TAP_INPUTS + 1;

constexpr std::size_t TWO_COMMANDS = 2;

/** @brief Offsets of a server position from the ship predicted after a
 * command: well inside the tolerance, and well beyond it. */
constexpr float INSIDE_THE_TOLERANCE = RECONCILIATION_TOLERANCE / 2.0F;
constexpr float BEYOND_THE_TOLERANCE = RECONCILIATION_TOLERANCE * 2.0F;

constexpr std::uint8_t OWN_PLAYER_NUMBER = 2;
constexpr std::uint8_t OTHER_PLAYER_NUMBER = 3;
constexpr std::uint32_t OWN_SHIP_ID = 7;
constexpr std::uint32_t OTHER_SHIP_ID = 8;
constexpr std::uint32_t NEWER_SHIP_ID = 9;
constexpr std::uint32_t ACKNOWLEDGED_COMMAND = 5;

constexpr engine::Duration HALF_A_TICK = engine::SIMULATION_TICK_DURATION / 2;

/** @brief How far through its roll of 400 ms a ship is after the one tick of
 * 16.67 ms of the command that starts it: 16.666666 / 400. */
constexpr float ONE_TICK_INTO_A_ROLL = 0.041666666F;

/** @brief Three ticks of commands, sent while the player had no ship. */
constexpr engine::Duration THREE_TICKS = engine::SIMULATION_TICK_DURATION * 3;
constexpr std::size_t TICKS_OF_A_STALL = 10;

/** @brief A tick of the server a client shows while it sends inputs. */
constexpr std::uint32_t SHOWN_TICK = 300;

/** @brief A blocking wall 64 by 256 centered at (600, 300), on the row of
 * START: its left side at x 568, which a ship touches with its center at
 * x 504 (568 - 64); and a second of commands, which take a ship holding right
 * from START far past it. */
constexpr engine::Bounds WALL_ON_THE_RIGHT{
    .centerX = 600.0F, .centerY = 300.0F, .width = 64.0F, .height = 256.0F};
constexpr float AGAINST_THE_WALL_ON_THE_RIGHT_X = 504.0F;
constexpr auto COMMANDS_OF_ONE_SECOND =
    static_cast<std::uint32_t>(engine::SIMULATION_TICKS_PER_SECOND);

}  // namespace rtype::client::testing
