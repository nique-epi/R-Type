#pragma once

#include <cstddef>
#include <vector>
#include "Endpoint.hpp"
#include "PacketHeader.hpp"

namespace rtype::network::testing {

/**
 * @returns A header of a session: the token SESSION_TOKEN, every other field
 * at its default.
 */
PacketHeader sessionHeader();

/**
 * @returns A header with no session, as a handshake datagram carries.
 */
PacketHeader handshakeHeader();

/**
 * @returns A WorldState datagram under @p header, holding the body of a
 * datagram the real server sent.
 */
std::vector<std::byte> worldStateDatagram(const PacketHeader& header);

/**
 * @returns A datagram under @p header whose body is random bytes, so coding
 * does not shrink it.
 */
std::vector<std::byte> incompressibleDatagram(const PacketHeader& header);

/**
 * @returns The coded form of the WorldState datagram under a session header.
 */
std::vector<std::byte> codedSessionDatagram();

/**
 * @returns The coded form of the WorldState datagram with its session token
 * set to 0, as a forger who knows no session would send it.
 */
std::vector<std::byte> codedDatagramWithoutSession();

/**
 * @returns The address of the peer on the loopback.
 */
Endpoint peer();

}  // namespace rtype::network::testing
