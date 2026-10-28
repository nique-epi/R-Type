#pragma once

#include <functional>
#include "EventBus.hpp"
#include "GameSettings.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SystemScheduler.hpp"
#include "TimerScheduler.hpp"
#include "WorldSlot.hpp"

namespace rtype::server {

/**
 * @brief What a game gives the modules it installs: the parts of the game they
 * work on. Every reference outlives the modules.
 */
struct GameContext {
  /** @brief Where the game publishes what happens in it. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief Runs what must happen later, such as the repeated copies of a
   * message. */
  std::reference_wrapper<engine::TimerScheduler> timers;
  /** @brief The systems that run on the world each tick; modules add theirs
   * when they are installed. */
  std::reference_wrapper<engine::SystemScheduler> systems;
  /** @brief The world of the game, which a module may replace. */
  std::reference_wrapper<WorldSlot> world;
  /** @brief The players of the game. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief The seed and the waves the game was defined with. */
  std::reference_wrapper<const GameSettings> settings;
  /** @brief What is called for each datagram to send. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief Whether at least one player is connected and every connected
   * player is ready. */
  std::function<bool()> everyPlayerIsReady;
};

}  // namespace rtype::server
