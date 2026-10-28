#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BoundedByte.hpp"
#include "MalformedDatagramKind.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "SerializationConstants.hpp"

namespace rtype::tools {

/**
 * @brief Every kind of malformed datagram, in the order a flood sends them.
 */
constexpr std::array MALFORMED_DATAGRAM_KINDS{
    MalformedDatagramKind::Truncated,   MalformedDatagramKind::TooLong,
    MalformedDatagramKind::Oversized,   MalformedDatagramKind::UnknownType,
    MalformedDatagramKind::LyingSize,   MalformedDatagramKind::ExtremeValue,
    MalformedDatagramKind::RandomBytes,
};

/**
 * @brief The name of each kind of MALFORMED_DATAGRAM_KINDS, at the same place,
 * as the report of a flood prints it.
 */
constexpr std::array<std::string_view, MALFORMED_DATAGRAM_KINDS.size()>
    MALFORMED_DATAGRAM_KIND_NAMES{
        "truncated",  "too long",      "oversized",    "unknown type",
        "lying size", "extreme value", "random bytes",
    };

/**
 * @brief The nickname of every ConnectionRequest the tool writes.
 */
constexpr std::string_view TOOL_NICKNAME = "MALFORMED";

/**
 * @brief The cookie of a first ConnectionRequest: 0 until the server gives
 * one, as the protocol page says.
 */
constexpr std::uint64_t NO_COOKIE = 0;

/**
 * @brief The values of the sample messages that need one: values the
 * protocol allows, which a malformed message never relies on.
 */
constexpr std::uint64_t SAMPLE_COOKIE = 1;
constexpr std::uint32_t SAMPLE_ENTITY_ID = 1;
constexpr std::uint16_t SAMPLE_EVENT_NUMBER = 1;
constexpr std::uint32_t SAMPLE_TICK = 1;

/**
 * @brief The smallest type byte an UnknownType message carries: one above
 * ConnectionChallenge, the highest type of version 0 on the protocol page.
 */
constexpr std::uint8_t FIRST_UNKNOWN_MESSAGE_TYPE =
    static_cast<std::uint8_t>(network::MessageType::ConnectionChallenge) + 1;

/**
 * @brief The most bytes the payload of an UnknownType message holds.
 */
constexpr std::size_t UNKNOWN_PAYLOAD_LIMIT = 64;

/**
 * @brief The value an ExtremeValue message writes in its bounded field. Every
 * field of BOUNDED_BYTES refuses it: a bool is 0 or 1, a player number 1 to 4,
 * a reason, a kind or an entity type 4 at most, the actions leave bits 5 to 7
 * at 0, a count of players is 4 at most, a nickname 16 bytes at most.
 */
constexpr std::uint8_t OUT_OF_RANGE_BYTE = 0xFF;

/**
 * @brief The one-byte field of each message that has one the protocol bounds,
 * in the order of the message types: the length of the nickname, the player
 * number, the reason, ready, the count of players, the actions, the count of
 * players, the entity type, the reason, the kind, the player number and the
 * count of players.
 */
constexpr std::array BOUNDED_BYTES{
    BoundedByte{.type = network::MessageType::ConnectionRequest,
                .offset = network::UINT64_WIRE_SIZE},
    BoundedByte{.type = network::MessageType::ConnectionAccepted, .offset = 0},
    BoundedByte{.type = network::MessageType::ConnectionRefused, .offset = 0},
    BoundedByte{.type = network::MessageType::ReadyState, .offset = 0},
    BoundedByte{.type = network::MessageType::LobbyState, .offset = 0},
    BoundedByte{.type = network::MessageType::PlayerInput,
                .offset = network::UINT32_WIRE_SIZE},
    BoundedByte{.type = network::MessageType::WorldState,
                .offset = network::UINT32_WIRE_SIZE},
    BoundedByte{.type = network::MessageType::EntitySpawned,
                .offset = network::UINT32_WIRE_SIZE},
    BoundedByte{.type = network::MessageType::EntityDestroyed,
                .offset = network::UINT32_WIRE_SIZE},
    BoundedByte{.type = network::MessageType::GameEvent,
                .offset = network::UINT16_WIRE_SIZE},
    BoundedByte{.type = network::MessageType::PlayerLeft, .offset = 0},
    BoundedByte{.type = network::MessageType::GameEnded, .offset = 0},
};

/**
 * @brief The size of the largest Oversized datagram: twice the largest one
 * the protocol allows, under the 9216 bytes macOS lets a UDP datagram hold by
 * default (`sysctl net.inet.udp.maxdgram`).
 */
constexpr std::size_t OVERSIZED_DATAGRAM_LIMIT = 2 * network::MAX_DATAGRAM_SIZE;

}  // namespace rtype::tools
