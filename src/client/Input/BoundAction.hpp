#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

/**
 * @brief What the player binds a key to: the four moves and the shot, which
 * become the actions of a PlayerInput, then ready, for the lobby.
 */
enum class BoundAction : std::uint8_t {
  MoveUp,
  MoveDown,
  MoveLeft,
  MoveRight,
  Shoot,
  Ready,
};

/** @brief Every bound action, in the order the options list them. */
constexpr std::array BOUND_ACTIONS{
    BoundAction::MoveUp,    BoundAction::MoveDown, BoundAction::MoveLeft,
    BoundAction::MoveRight, BoundAction::Shoot,    BoundAction::Ready};

constexpr std::size_t BOUND_ACTION_COUNT = BOUND_ACTIONS.size();

}  // namespace rtype::client
