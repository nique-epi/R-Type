#pragma once

#include "Direction.hpp"

namespace rtype::game {

/**
 * @brief Component of a body that a controller steers: what the controller
 * wants the body to do at this tick.
 *
 * The controller, a player or an artificial intelligence, writes it; the
 * systems that move and arm the body read it, and never know which controller
 * wrote it. Nothing is wanted by default.
 */
struct Intent {
  /** @brief Where the controller wants the body to go: its share of the
   * full speed of the body on each axis. */
  Direction direction{};
  /** @brief Whether the controller wants the body to fire its Gun. */
  bool shooting{false};
  /** @brief Whether a controller wrote this Intent since the body appeared: a
   * new body has been told nothing yet, so what it holds is not a decision. */
  bool steered{false};
};

}  // namespace rtype::game
