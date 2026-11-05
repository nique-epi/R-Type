#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include "PlayerAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

constexpr std::uint32_t PILOT_SEED = 7;

/** @brief A seed other than PILOT_SEED, as the next bot of a squad draws
 * from. */
constexpr std::uint32_t OTHER_PILOT_SEED = 8;

/** @brief A minute of frames of SIMULATION_TICK_DURATION. */
constexpr std::size_t FRAMES_IN_A_MINUTE = 3600;

/** @brief The shortest time a bot holds an action, as the tool requires. */
constexpr engine::Duration A_FIFTH_OF_A_SECOND = std::chrono::milliseconds{200};

/** @brief The actions a bot may hold, as the tool requires: the four
 * directions and shoot. */
constexpr std::array EVERY_BOT_ACTION{
    client::PlayerAction::Up, client::PlayerAction::Down,
    client::PlayerAction::Left, client::PlayerAction::Right,
    client::PlayerAction::Shoot};

}  // namespace rtype::tools::testing
