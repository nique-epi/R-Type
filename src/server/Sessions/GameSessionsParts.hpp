#pragma once

#include <functional>
#include "ChatLine.hpp"
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "IClock.hpp"
#include "LeaderboardRequest.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerInput.hpp"

namespace rtype::server {

/** @brief What a GameSessions is built from. */
struct GameSessionsParts {
  /** @brief Where a departure is published; it must outlive the sessions. */
  std::reference_wrapper<engine::EventBus> events;
  /** @brief What is called for each datagram to send. Must not be empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief Read at the start and the end of each tick for the Heartbeats
   * (see ConnectedPlayer::sinceHeartbeat); it must outlive the sessions. */
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
  /** @brief Called with the player and each LeaderboardRequest it sends, for
   * whoever answers it (the results of the game). Must not be empty. */
  std::function<void(const ConnectedPlayer&,
                     const network::LeaderboardRequest&)>
      onLeaderboardRequest;
  /** @brief Called with the player and each ChatLine it sends, copies included,
   * for whoever relays it (the chat of the game). Must not be empty. */
  std::function<void(const ConnectedPlayer&, const network::ChatLine&)>
      onChatLine;
};

}  // namespace rtype::server
