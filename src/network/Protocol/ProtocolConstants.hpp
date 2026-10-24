#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "NetworkConstants.hpp"
#include "SerializationConstants.hpp"
#include "WorldState.hpp"

namespace rtype::network {

constexpr std::size_t INT16_WIRE_SIZE = sizeof(std::int16_t);

/**
 * @brief The first four bytes of every datagram, "RTYP" in ASCII.
 */
constexpr std::uint32_t PROTOCOL_ID = 0x52545950;

/**
 * @brief The version of the protocol this code speaks.
 */
constexpr std::uint8_t PROTOCOL_VERSION = 0;

/**
 * @brief The session token of a datagram sent before the connection is
 * accepted, such as a ConnectionRequest. The server never draws it.
 */
constexpr std::uint64_t NO_SESSION_TOKEN = 0;

/**
 * @brief The values a message type byte can hold, so the size of a table
 * indexed by it.
 */
constexpr std::size_t MESSAGE_TYPE_COUNT =
    std::numeric_limits<std::uint8_t>::max() + 1;

/**
 * @brief The two values a boolean takes on the wire; any other byte is
 * invalid.
 */
constexpr std::uint8_t FALSE_WIRE_VALUE = 0;
constexpr std::uint8_t TRUE_WIRE_VALUE = 1;

/**
 * @brief Bytes of the packet header every datagram starts with, see the
 * Datagram section of the protocol page.
 */
constexpr std::size_t PACKET_HEADER_SIZE = 22;

static_assert(PACKET_HEADER_SIZE == UINT32_WIRE_SIZE + UINT8_WIRE_SIZE +
                                        UINT8_WIRE_SIZE + UINT64_WIRE_SIZE +
                                        UINT16_WIRE_SIZE + UINT16_WIRE_SIZE +
                                        UINT32_WIRE_SIZE,
              "the header is protocolId, version, flags, sessionToken, "
              "sequence, acknowledgedSequence and acknowledgementBits");

/**
 * @brief Bytes before the payload of each message: its type (one byte) and its
 * payload size (two bytes).
 */
constexpr std::size_t MESSAGE_PREFIX_SIZE = UINT8_WIRE_SIZE + UINT16_WIRE_SIZE;

/**
 * @brief The most bytes the payload of one message holds: a datagram, less
 * the packet header and the message prefix that go around it.
 */
constexpr std::size_t MESSAGE_PAYLOAD_LIMIT =
    MAX_DATAGRAM_SIZE - PACKET_HEADER_SIZE - MESSAGE_PREFIX_SIZE;

static_assert(MESSAGE_PAYLOAD_LIMIT <=
                  std::numeric_limits<std::uint16_t>::max(),
              "the size of a payload is sent on two bytes");

/**
 * @brief Players a game holds, so the most a WorldState lists.
 */
constexpr std::size_t MAXIMUM_PLAYERS = 4;

/**
 * @brief The number of the first player of a game; the last is
 * MAXIMUM_PLAYERS.
 */
constexpr std::uint8_t FIRST_PLAYER_NUMBER = 1;

/**
 * @brief The player number of a GameEvent that names no player.
 */
constexpr std::uint8_t NO_PLAYER_NUMBER = 0;

/**
 * @brief The actions a PlayerInput carries, one bit each from bit 0: up,
 * down, left, right, shoot. The bits above them are 0.
 */
constexpr std::size_t PLAYER_ACTION_COUNT = 5;
constexpr auto PLAYER_ACTION_BITS =
    static_cast<std::uint8_t>((1U << PLAYER_ACTION_COUNT) - 1);

/**
 * @brief Where each action sits in the actions byte of a PlayerInput, counted
 * from bit 0, the least significant.
 */
constexpr std::uint8_t UP_ACTION_BIT_POSITION = 0;
constexpr std::uint8_t DOWN_ACTION_BIT_POSITION = 1;
constexpr std::uint8_t LEFT_ACTION_BIT_POSITION = 2;
constexpr std::uint8_t RIGHT_ACTION_BIT_POSITION = 3;
constexpr std::uint8_t SHOOT_ACTION_BIT_POSITION = 4;

/**
 * @brief The inputNumber of the first PlayerInput of a session; each next one
 * is one more.
 */
constexpr std::uint32_t FIRST_INPUT_NUMBER = 1;

/**
 * @brief The longest time either side of a session lets pass without sending
 * its peer anything: once it passed, a Heartbeat goes out, so the peer knows
 * the sender is still there. Provisional, see the protocol page.
 */
constexpr std::chrono::seconds HEARTBEAT_INTERVAL{1};

/**
 * @brief The longest a side of a session waits for a datagram of its peer
 * before it takes the peer for gone. Provisional, see the protocol page.
 */
constexpr std::chrono::seconds SESSION_TIMEOUT{5};

/**
 * @brief Bytes of a player in a WorldState: playerNumber, shipEntityId, lives,
 * score and invulnerable.
 */
constexpr std::size_t PLAYER_STATE_WIRE_SIZE =
    UINT8_WIRE_SIZE + UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + UINT32_WIRE_SIZE +
    UINT8_WIRE_SIZE;

/**
 * @brief Bytes of an entity in a WorldState: entityId, entityType, x and y.
 */
constexpr std::size_t ENTITY_STATE_WIRE_SIZE =
    UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + INT16_WIRE_SIZE + INT16_WIRE_SIZE;

/**
 * @brief Bytes of a WorldState that do not depend on its players and entities:
 * tick, playerCount and entityCount.
 */
constexpr std::size_t WORLD_STATE_FIXED_SIZE =
    UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + UINT8_WIRE_SIZE;

/**
 * @brief The sizes, in bytes, a nickname may have. A letter without accent
 * takes one byte, an accented letter two.
 */
constexpr std::size_t MINIMUM_NICKNAME_SIZE = 1;
constexpr std::size_t MAXIMUM_NICKNAME_SIZE = 16;

/**
 * @brief The control characters a nickname may not hold are the bytes below
 * SPACE_BYTE, U+0000 to U+001F, and DELETE_BYTE, U+007F. In UTF-8 each of them
 * is one byte of its own value, and every byte of a longer sequence is 0x80 or
 * above, so reading the bytes finds exactly these characters.
 */
constexpr std::uint8_t SPACE_BYTE = 0x20;
constexpr std::uint8_t DELETE_BYTE = 0x7F;

/**
 * @brief The size of the smallest datagram holding a ConnectionRequest: a
 * cookie and a nickname of one byte.
 */
constexpr std::size_t SMALLEST_CONNECTION_REQUEST_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT64_WIRE_SIZE +
    SHORT_STRING_PREFIX_SIZE + MINIMUM_NICKNAME_SIZE;

/**
 * @brief The size of the largest datagram the server sends a sender it does
 * not know yet: a ConnectionChallenge and its cookie. ConnectionRefused and
 * ConnectionAccepted carry one byte.
 */
constexpr std::size_t LARGEST_CONNECTION_ANSWER_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT64_WIRE_SIZE;

static_assert(LARGEST_CONNECTION_ANSWER_DATAGRAM_SIZE <=
                  SMALLEST_CONNECTION_REQUEST_DATAGRAM_SIZE,
              "the server never sends a sender it does not know more bytes "
              "than the request it answers");

/**
 * @brief The size of a datagram holding a ConnectionRefused and its reason:
 * the server answers a datagram of another version from a sender it does not
 * know only when it holds at least that many bytes, so the answer is never
 * larger than what it answers.
 */
constexpr std::size_t CONNECTION_REFUSED_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT8_WIRE_SIZE;

/**
 * @brief The names of the fields a refused message is reported by, as the
 * protocol page writes them.
 */
constexpr std::string_view PLAYER_NUMBER_FIELD = "playerNumber";
constexpr std::string_view ENTITY_ID_FIELD = "entityId";
constexpr std::string_view ENTITY_TYPE_FIELD = "entityType";
constexpr std::string_view REASON_FIELD = "reason";
constexpr std::string_view KIND_FIELD = "kind";
constexpr std::string_view ACTIONS_FIELD = "actions";

/**
 * @brief Wire steps in one logical unit: a coordinate is sent in eighths of a
 * unit.
 */
constexpr float WIRE_COORDINATE_STEPS_PER_UNIT = 8.0F;

/**
 * @brief Bytes a WorldState payload may hold: as many as any message.
 */
constexpr std::size_t WORLD_STATE_PAYLOAD_LIMIT = MESSAGE_PAYLOAD_LIMIT;

static_assert((WORLD_STATE_PAYLOAD_LIMIT - WORLD_STATE_FIXED_SIZE) /
                      ENTITY_STATE_WIRE_SIZE <=
                  std::numeric_limits<std::uint8_t>::max(),
              "entityCount is one byte: it must hold what a datagram fits");

/**
 * @brief The smallest and the largest value of an entity type on the wire.
 */
constexpr std::uint8_t FIRST_WIRE_ENTITY_TYPE =
    static_cast<std::uint8_t>(WireEntityType::PlayerShip);
constexpr std::uint8_t LAST_WIRE_ENTITY_TYPE =
    static_cast<std::uint8_t>(WireEntityType::EnemyMissile);

/**
 * @brief The smallest and the largest count of eighths of a unit a wire
 * coordinate holds, that is -4096 and 4095.875 units.
 */
constexpr float MINIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::min());
constexpr float MAXIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::max());

}  // namespace rtype::network
