#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "EntityTypeEntry.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ShipGun.hpp"

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
  /** @brief The entity types of the game, entry n - 1 for the type numbered
   * n, which every ConnectionAccepted carries (see entityTypeTableOf()). */
  std::vector<network::EntityTypeEntry> entityTypes;
  /** @brief The gun of the ships, which every ConnectionAccepted carries after
   * the types (see shipGunOf()). */
  network::ShipGun shipGun{};
};

}  // namespace rtype::server
