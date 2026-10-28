#pragma once

#include <cstdint>
#include <functional>
#include "EventBus.hpp"
#include "IGameReplication.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SystemScheduler.hpp"
#include "WavePlan.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

/** @brief What a GameFlow is built from. */
struct GameFlowParts {
  /** @brief Where the rules of the world publish; it must outlive the flow. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief The systems that run on the world each tick, which the flow runs
   * and whose owner added them; it must outlive the flow. */
  std::reference_wrapper<engine::SystemScheduler> systems;
  /** @brief Holds the world of the game, which the flow replaces with each new
   * world; it must outlive the flow. */
  std::reference_wrapper<WorldSlot> world;
  /** @brief The players of the game; it must outlive the flow. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief Tells the players how the game stands, and gives them their ships;
   * it must outlive the flow. */
  std::reference_wrapper<IGameReplication> replication;
  /** @brief What is called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief Tells whether at least one player is connected and every
   * connected player is ready: the rule that starts a game, which belongs to
   * the sessions. Must not be empty. */
  std::function<bool()> everyPlayerIsReady;
  /** @brief The seed the random draws of the game start from, logged by the
   * spawner each time a world is built. */
  std::uint32_t seed;
  /** @brief The waves of the difficulty of the game, which must hold a
   * playable curve and at least one enemy; they must outlive the flow. */
  std::reference_wrapper<const WavePlan> waves;
};

}  // namespace rtype::server
