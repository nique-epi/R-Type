#pragma once

#include <functional>
#include "EventBus.hpp"
#include "GameSettings.hpp"
#include "IClock.hpp"
#include "OutgoingDatagram.hpp"
#include "TimerScheduler.hpp"

namespace rtype::server {

/** @brief What a GameInstance is built from, besides its modules. */
struct GameInstanceParts {
  /** @brief What is called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief The seed the random draws of the game start from, and the waves of
   * its difficulty, which must hold a playable curve and at least one enemy. */
  GameSettings settings;
  /** @brief Where the game publishes what happens in it; it must outlive the
   * game, and its owner dispatches it after each onTick(). */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief Runs what the modules plan for later; it must outlive the game,
   * and its owner advances it after each onTick(). */
  std::reference_wrapper<engine::TimerScheduler> timers;
  /** @brief Read at the start of each tick for the Heartbeats (see
   * ConnectedPlayer::sinceSent); it must outlive the game. */
  std::reference_wrapper<const engine::IClock> clock;
};

}  // namespace rtype::server
