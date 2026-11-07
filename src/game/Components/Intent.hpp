#pragma once

#include <optional>
#include "ActionKey.hpp"
#include "CollisionBox.hpp"
#include "Direction.hpp"
#include "Position.hpp"

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
  /** @brief The attack and the phase an enemy enters at this tick, for
   * PilotSystem to record and announce with startEnemyAction(); nothing for a
   * tick that enters none. */
  std::optional<ActionKey> startsAction{std::nullopt};
  /** @brief Whether the enemy leaves its action at this tick, for PilotSystem
   * to record with endEnemyAction(). When it also starts one, it leaves the
   * old one first. */
  bool endsAction{false};
  /** @brief Where the body appears at this tick, without travelling there:
   * PilotSystem moves it after the behaviour steered; nothing for a body
   * that only moves by its direction. */
  std::optional<Position> jumpsTo{std::nullopt};
  /** @brief The collision box the body takes at this tick, which
   * PilotSystem applies to a body that has one; nothing to keep its box. */
  std::optional<CollisionBox> resizesTo{std::nullopt};
  /** @brief The top speed the body takes at this tick, in logical units per
   * second, which PilotSystem applies to a body with a SteadyMotion; nothing
   * to keep its speed. */
  std::optional<float> topSpeedTo{std::nullopt};
};

}  // namespace rtype::game
