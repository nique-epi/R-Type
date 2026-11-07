#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::client::testing {

/** @brief Where the predicted ship stands when it fires, and where its missile
 * leaves the muzzle: the missile of 64 units ahead of the ship of 128, half
 * of each, so 96 units to the right. */
constexpr game::Position SHIP_WHEN_IT_FIRES{.x = 200.0F, .y = 300.0F};
constexpr game::Position MISSILE_AT_THE_MUZZLE{.x = 296.0F, .y = 300.0F};

/** @brief A missile flies 16 units a tick at 960 units per second: three
 * ticks after the input that fired it, 48 units further. */
constexpr std::uint32_t FIRING_INPUT = 1;
constexpr std::uint32_t THREE_TICKS_LATER_INPUT = 4;
constexpr float THREE_TICKS_OF_FLIGHT = 48.0F;

/** @brief Where the server put the missile it fired, 4 units ahead of the
 * prediction, and its network id. */
constexpr game::Position SERVER_START{.x = 300.0F, .y = 300.0F};
constexpr std::uint32_t SERVER_MISSILE_ID = 77;

/** @brief The input after which the server applied 14 more, the most a shot
 * waits for its EntitySpawned. */
constexpr std::uint32_t INPUT_THAT_REFUSES_THE_FIRST_SHOT = 15;
constexpr std::uint32_t INPUT_STILL_WAITING_FOR_IT = 14;

/** @brief The player of the tests. */
constexpr std::uint8_t SHOOTING_PLAYER = 1;

}  // namespace rtype::client::testing
