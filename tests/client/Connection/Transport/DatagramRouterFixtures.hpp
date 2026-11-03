#pragma once

#include <cstddef>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::client::testing {

/** @returns The game server: GAME_SERVER_ADDRESS, GAME_SERVER_PORT. */
network::Endpoint gameServer();

/** @returns A master server on the address of the game server, on
 * MASTER_SERVER_PORT. */
network::Endpoint masterServer();

/** @returns A host no route listens to. */
network::Endpoint stranger();

/** @returns A datagram of one byte, @p tag, sent by @p sender. */
network::IncomingDatagram datagramFrom(const network::Endpoint& sender,
                                       std::byte tag);

/** @returns A datagram of one byte, @p tag, for @p destination. */
network::OutgoingDatagram datagramTo(const network::Endpoint& destination,
                                     std::byte tag);

/** @returns The datagrams of @p sender carrying the tags @p tags, in that
 * order. */
std::vector<network::IncomingDatagram> datagramsFrom(
    const network::Endpoint& sender, const std::vector<std::byte>& tags);

}  // namespace rtype::client::testing
