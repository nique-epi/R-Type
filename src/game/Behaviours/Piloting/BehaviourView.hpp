#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include "ActionKey.hpp"
#include "IBehaviourState.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief What a behaviour may know of the body it pilots, at one tick.
 */
struct BehaviourView {
  /** @brief Where the center of the body is now, before this tick moves it. */
  Position position{};
  /** @brief Where the center of the body was when it appeared. */
  Position origin{};
  /** @brief How long the body has existed, this tick included. */
  engine::Duration age{};
  /** @brief How long this tick lasts. */
  engine::Duration elapsed{};
  /** @brief The speed of the body on each axis at a full Direction, in logical
   * units per second; 0 for a body with no SteadyMotion. */
  float topSpeed{};
  /** @brief The values the sheet of the body gives the settings of its
   * behaviour, in the order IBehaviour::settings() declares them (see
   * settingAt()). */
  std::span<const std::int64_t> settings;
  /** @brief The attack and phase the body is in (its CurrentAction); nothing
   * while it is idle, and for a body without CurrentAction. */
  std::optional<ActionKey> action{std::nullopt};
  /** @brief How long the body has been in action, or idle since its last
   * action ended, this tick included; zero for a body without
   * CurrentAction. */
  engine::Duration actionAge{};
  /** @brief The center of the ship of a player nearest to the body; nothing
   * while no ship is in play. */
  std::optional<Position> nearestShip{std::nullopt};
  /** @brief Whether the body has lost health points since it appeared;
   * false for a body without Health. */
  bool wounded{false};
  /** @brief The share of its health points the body has left, 0 to 1; 1 for
   * a body without Health. */
  float healthShare{1.0F};
  /** @brief Whether the body is a Boss that enraged (see BossPhaseSystem);
   * false for any other body. */
  bool enraged{false};
  /** @brief How many actions the body has ended since it appeared
   * (CurrentAction::ended); 0 for a body without CurrentAction. */
  std::uint32_t actionsEnded{0};
  /** @brief The network id of the body; 0 for a body without one. */
  std::uint32_t networkId{0};
  /** @brief Where the behaviour keeps what it remembers of the body; nullptr
   * when nothing outlives this call. */
  std::shared_ptr<IBehaviourState>* behaviourState{nullptr};
};

}  // namespace rtype::game
