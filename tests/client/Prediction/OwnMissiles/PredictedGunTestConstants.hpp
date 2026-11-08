#pragma once

#include <cstddef>
#include <cstdint>
#include "ShipGun.hpp"

namespace rtype::client::testing {

/** @brief The gun of the shipped ship: one shot every 20 ticks, the first at
 * once. */
constexpr game::ShipGun READY_GUN{.shipType = 4,
                                  .projectileType = 3,
                                  .intervalTicks = 20,
                                  .firstShotTicks = 0,
                                  .velocityX = 960.0F,
                                  .velocityY = 0.0F};

/** @brief Commands held for 41 ticks fire on the first, the 21st and the
 * 41st; the server fired on the third. */
constexpr std::size_t COMMANDS_OF_THREE_SHOTS = 41;
constexpr std::uint32_t SECOND_SHOT_INPUT = 21;
constexpr std::uint32_t THIRD_SHOT_INPUT = 41;
constexpr std::uint32_t INPUT_THE_SERVER_FIRED = 3;
constexpr std::uint32_t SHOT_AFTER_THE_SERVER_ONE = 23;

/** @brief Commands kept after the shot of the server, past its next shot. */
constexpr std::size_t THIRTY_COMMANDS = 30;
constexpr std::uint32_t THIRTIETH_INPUT = 30;

/** @brief Commands sent before a ship was known: the server applied up to
 * the second when its state listed the ship, so its first shot is the
 * third. */
constexpr std::size_t FIVE_COMMANDS = 5;
constexpr std::uint32_t ACKNOWLEDGED_BEFORE_THE_SHIP = 2;
constexpr std::uint32_t FIRST_SHOT_OF_THE_SHIP = 3;

}  // namespace rtype::client::testing
