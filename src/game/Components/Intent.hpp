#pragma once

namespace rtype::game {

/**
 * @brief The directions a player holds for its ship. Holding two opposite
 * directions is holding neither.
 */
struct ShipDirections {
  bool up{false};
  bool down{false};
  bool left{false};
  bool right{false};
};

/**
 * @brief Component of a body that a controller steers: what the controller
 * wants the body to do at this tick.
 *
 * The controller, a player or an artificial intelligence, writes it; the
 * systems that move and arm the body read it, and never know which controller
 * wrote it. Nothing is wanted by default.
 */
struct Intent {
  /** @brief The directions the controller wants the body to go in. */
  ShipDirections directions{};
  /** @brief Whether the controller wants the body to fire its Gun. */
  bool shooting{false};
  /** @brief Whether a controller wrote this Intent since the body appeared: a
   * new body has been told nothing yet, so what it holds is not a decision. */
  bool steered{false};
};

}  // namespace rtype::game
