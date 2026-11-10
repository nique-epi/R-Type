#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ProtocolConstants.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network::testing {

/**
 * @brief A session token, as the server writes in every datagram of a session.
 */
constexpr std::uint64_t SESSION_TOKEN = 0x0102030405060708;

/**
 * @brief Where the session token sits in a datagram, after the protocol id,
 * the version and the flags.
 */
constexpr std::size_t TOKEN_OFFSET = FLAGS_OFFSET + UINT8_WIRE_SIZE;

/**
 * @brief The bytes of the session token in a datagram.
 */
constexpr std::size_t SESSION_TOKEN_SIZE = UINT64_WIRE_SIZE;

/**
 * @brief Where the version sits in a datagram, after the protocol id.
 */
constexpr std::size_t VERSION_OFFSET = UINT32_WIRE_SIZE;

/**
 * @brief The version of a peer that speaks another protocol than this one.
 */
constexpr std::uint8_t ANOTHER_VERSION = PROTOCOL_VERSION + 1;

/**
 * @brief The port of the peer that sends or receives the datagrams.
 */
constexpr std::uint16_t PEER_PORT = 40000;

/**
 * @brief The bytes of the body of a datagram that does not shrink when coded:
 * random bytes, which cost 8 bits or more each.
 */
constexpr std::size_t INCOMPRESSIBLE_BODY_SIZE = 200;

/**
 * @brief A nickname of the longest size, one letter repeated, so a status
 * naming it four times is a body zstd shortens.
 */
constexpr std::string_view REPEATED_NICKNAME = "AAAAAAAAAAAAAAAA";

}  // namespace rtype::network::testing
