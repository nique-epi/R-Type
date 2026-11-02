#pragma once

#include <functional>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "IClock.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerInput.hpp"

namespace rtype::server {

/** @brief What a GameSessions is built from. */
struct GameSessionsParts {
  /** @brief Where a departure is published; it must outlive the sessions. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief What is called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief Read at the start of each tick (see ConnectedPlayer::sinceSent);
   * it must outlive the sessions. */
  std::reference_wrapper<const engine::IClock> clock;
  /** @brief Tells whether a ReadyState counts right now. Must not be empty. */
  std::function<bool()> readyAccepted;
  /** @brief Called with the player whose session closes, after its seat is
   * freed and before its departure is announced, for whoever owns something of
   * the player (its ship). Must not be empty. */
  std::function<void(const ConnectedPlayer&)> onClosing;
  /** @brief Called with the player and each PlayerInput newer than the one
   * kept, before it replaces it, for whoever reads the presses an input
   * holds (the game reads the double presses of a ship there). Must not be
   * empty. */
  std::function<void(const ConnectedPlayer&, const network::PlayerInput&)>
      onNewerInput;
};

}  // namespace rtype::server
