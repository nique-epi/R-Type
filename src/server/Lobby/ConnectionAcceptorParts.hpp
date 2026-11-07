#pragma once

#include <cstdint>
#include <functional>
#include "ConnectedPlayer.hpp"
#include "ContentAnnouncement.hpp"
#include "Endpoint.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/** @brief What a ConnectionAcceptor is built from. */
struct ConnectionAcceptorParts {
  /** @brief The players of the game; it must outlive the acceptor. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What the acceptor calls for each answer to send. Must not be
   * empty. */
  std::function<void(network::OutgoingDatagram)> send;
  /** @brief What the acceptor calls once for each sender it gives a place,
   * with the session token it drew, so the datagrams of that player carrying
   * that token reach the game from then on. Must not be empty. */
  std::function<void(const network::Endpoint&, std::uint64_t)> admit;
  /** @brief What the acceptor calls once for each player it lets in, after it
   * sent the acceptance; not when an acceptance is repeated. Empty to call
   * nothing. */
  std::function<void(ConnectedPlayer&)> onPlayerJoined;
  /** @brief The content the game plays with, which every ConnectionAccepted
   * announces (see contentAnnouncementOf()). */
  network::ContentAnnouncement content;
};

}  // namespace rtype::server
