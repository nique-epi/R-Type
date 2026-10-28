#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server {

/** @brief What a GameSessions is built from. */
struct GameSessionsParts {
  /** @brief Where a departure is published; it must outlive the sessions. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief What is called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief Tells whether a ReadyState counts right now. Must not be empty. */
  std::function<bool()> readyAccepted;
  /** @brief Called with the player whose session closes, after its seat is
   * freed and before its departure is announced, for whoever owns something of
   * the player (its ship). Must not be empty. */
  std::function<void(const ConnectedPlayer&)> onClosing;
};

}  // namespace rtype::server
