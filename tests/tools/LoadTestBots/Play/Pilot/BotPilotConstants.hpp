#pragma once

#include <array>
#include <chrono>
#include "PlayerAction.hpp"
#include "TimeConstants.hpp"

namespace rtype::tools {

/** @brief What a bot holds, one at a time: a direction, or shoot. */
constexpr std::array BOT_ACTIONS{
    client::PlayerAction::Up, client::PlayerAction::Down,
    client::PlayerAction::Left, client::PlayerAction::Right,
    client::PlayerAction::Shoot};

/**
 * @brief How long a bot holds the action it drew: a time drawn from
 * SHORTEST_HOLD to LONGEST_HOLD at each draw, so it moves across the
 * playfield rather than shaking in place.
 */
constexpr engine::Duration SHORTEST_HOLD = std::chrono::milliseconds{200};
constexpr engine::Duration LONGEST_HOLD = std::chrono::seconds{1};

}  // namespace rtype::tools
