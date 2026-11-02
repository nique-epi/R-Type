#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "BarState.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A call to act on a screen, as paintGlowingCall() writes it. */
struct GlowingCall {
  /** @brief The words of the call, UTF-8. */
  std::string_view text;
  /** @brief Where its first letter starts while it is not pressed. */
  sf::Vector2i topLeft;
  /** @brief Time since its first letter was typed; negative before. */
  engine::Duration sinceTyping{};
  /** @brief At rest, under the pointer, or pressed. */
  BarState state = BarState::Idle;
  /** @brief Whether it never blinks: when the player reduced motion or
   * flashes. */
  bool steady = false;
};

}  // namespace rtype::client
