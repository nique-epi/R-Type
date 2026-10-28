#pragma once

#include <functional>
#include "EventBus.hpp"
#include "GameSettings.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/** @brief What a GameFlow is built from. */
struct GameFlowParts {
  /** @brief Where the rules of the world publish; it must outlive the flow. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The players of the game; it must outlive the flow. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief Tells whether at least one player is connected and every
   * connected player is ready: the rule that starts a game, which belongs to
   * the sessions. Must not be empty. */
  std::function<bool()> everyPlayerIsReady;
  /** @brief The seed the random draws of the game start from, logged by the
   * spawner each time a world is built, and the waves of its difficulty, which
   * must hold a playable curve and at least one enemy. */
  GameSettings settings;
};

}  // namespace rtype::server
