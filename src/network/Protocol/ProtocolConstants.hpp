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
 * @brief The version of the protocol this code speaks. Version 3 adds the
 * outcome of a game to GameEnded, see the Version 3 section of the protocol
 * page.
 */
constexpr std::uint8_t PROTOCOL_VERSION = 3;

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
 * @brief The flags of the packet header, one bit each. ACKNOWLEDGES_FLAG says
 * that acknowledgedSequence and acknowledgementBits name the datagrams the
 * sender accepted from its peer. GUARANTEED_FLAG says that the datagram holds
 * guaranteed messages, which its receiver acknowledges within
 * ACKNOWLEDGEMENT_DELAY. A header with a bit outside KNOWN_FLAGS is not of this
 * version.
 */
constexpr std::uint8_t ACKNOWLEDGES_FLAG = 0x01;
constexpr std::uint8_t GUARANTEED_FLAG = 0x02;
constexpr auto KNOWN_FLAGS =
    static_cast<std::uint8_t>(ACKNOWLEDGES_FLAG | GUARANTEED_FLAG);

/**
 * @brief The datagrams before acknowledgedSequence that acknowledgementBits
 * names, one bit each: bit 0 for the one just before, bit 31 for the one 32
 * before.
 */
constexpr std::size_t ACKNOWLEDGEMENT_BIT_COUNT =
    std::numeric_limits<std::uint32_t>::digits;

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
 * @brief The directions among the actions, bits 0 to 3: up, down, left and
 * right. A PlayerState repeats them in its motion.
 */
constexpr std::size_t DIRECTION_ACTION_COUNT = 4;
constexpr auto DIRECTION_ACTION_BITS =
    static_cast<std::uint8_t>((1U << DIRECTION_ACTION_COUNT) - 1);

/**
 * @brief The bits of the motion of a PlayerState after its directions: its
 * ship rolls, its ship dashes. Bits 6 and 7 are 0.
 */
constexpr std::uint8_t ROLLING_MOTION_BIT_POSITION = 4;
constexpr std::uint8_t DASHING_MOTION_BIT_POSITION = 5;
constexpr std::size_t PLAYER_MOTION_BIT_COUNT = 6;
constexpr auto PLAYER_MOTION_BITS =
    static_cast<std::uint8_t>((1U << PLAYER_MOTION_BIT_COUNT) - 1);

/**
 * @brief The inputNumber of the first PlayerInput of a session; each next one
 * is one more.
 */
constexpr std::uint32_t FIRST_INPUT_NUMBER = 1;

/**
 * @brief The time between two Heartbeats either side of a session sends its
 * peer, whatever else it sends: every one tells the peer the sender is still
 * there, and the peer echoes the newest one in its own, which measures the
 * round trip (see RoundTripMeter). Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds HEARTBEAT_INTERVAL{500};

/**
 * @brief The longest a side of a session waits for a datagram of its peer
 * before it takes the peer for gone. Provisional, see the protocol page.
 */
constexpr std::chrono::seconds SESSION_TIMEOUT{5};

/**
 * @brief How long the server keeps the seat of a player it took for gone,
 * counted from SESSION_TIMEOUT: a datagram of the player carrying its session
 * token, from the same address and port, within this delay gives it back its
 * seat, its ship, its score and its lives; past it the seat is freed.
 * Provisional, see the protocol page.
 */
constexpr std::chrono::seconds RECONNECTION_GRACE{15};

/**
 * @brief The silence after which the server closes the session of a player,
 * and a client takes its server for gone: SESSION_TIMEOUT plus
 * RECONNECTION_GRACE. Within it, a side keeps the session and sends its
 * heartbeats, so a cut of the network that ends sooner costs nothing.
 */
constexpr std::chrono::seconds SESSION_SILENCE_LIMIT =
    SESSION_TIMEOUT + RECONNECTION_GRACE;

/**
 * @brief The least time between two sends of a guaranteed message its receiver
 * has not acknowledged, the resend time of the reliable channel of yojimbo.
 * Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds GUARANTEED_RESEND_DELAY{100};

/**
 * @brief How long a guaranteed message is sent again while it is not
 * acknowledged, counted from the moment it was given; past it, it is dropped,
 * since what it shows no longer holds: the client removes an entity no
 * WorldState lists after UNLISTED_ENTITY_TIMEOUT. Provisional, see the protocol
 * page.
 */
constexpr std::chrono::seconds GUARANTEED_MESSAGE_LIFETIME{1};

/**
 * @brief The longest a receiver keeps the acknowledgement of a datagram that
 * holds guaranteed messages before it sends a Heartbeat to carry it, when no
 * other datagram went out meanwhile. It is longer than the time between two
 * PlayerInput at 30 frames per second and more, so during a game the next
 * input carries it. Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds ACKNOWLEDGEMENT_DELAY{40};

static_assert(ACKNOWLEDGEMENT_DELAY < GUARANTEED_RESEND_DELAY,
              "an acknowledgement goes back before the message is sent again");
static_assert(GUARANTEED_RESEND_DELAY < GUARANTEED_MESSAGE_LIFETIME,
              "a guaranteed message is sent more than once in its lifetime");

/**
 * @brief How long the client keeps an entity no WorldState lists any more:
 * past it, the server destroyed the entity and its EntityDestroyed never
 * arrived, so the client removes it, without an effect.
 * Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds UNLISTED_ENTITY_TIMEOUT{900};

/**
 * @brief Bytes of a player in a WorldState: playerNumber, shipEntityId, lives,
 * score, invulnerable, motion and acknowledgedInput.
 */
constexpr std::size_t PLAYER_STATE_WIRE_SIZE =
    UINT8_WIRE_SIZE + UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + UINT32_WIRE_SIZE +
    UINT8_WIRE_SIZE + UINT8_WIRE_SIZE + UINT32_WIRE_SIZE;

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
 * @brief The rows a LeaderboardAnswer lists at most, its own row aside.
 */
constexpr std::size_t MAXIMUM_LEADERBOARD_ROWS = 10;

/**
 * @brief The size of a row of a LeaderboardAnswer with the longest nickname:
 * rank, score, date and nickname.
 */
constexpr std::size_t LARGEST_LEADERBOARD_ROW_SIZE =
    UINT32_WIRE_SIZE + UINT32_WIRE_SIZE + UINT64_WIRE_SIZE +
    SHORT_STRING_PREFIX_SIZE + MAXIMUM_NICKNAME_SIZE;

/**
 * @brief The size of the largest payload of a LeaderboardAnswer: the
 * leaderboard asked for, the count of rows, every row, then the flag and the
 * row of the player.
 */
constexpr std::size_t LARGEST_LEADERBOARD_ANSWER_SIZE =
    UINT8_WIRE_SIZE + SHORT_STRING_PREFIX_SIZE + MAXIMUM_NICKNAME_SIZE +
    UINT8_WIRE_SIZE +
    (MAXIMUM_LEADERBOARD_ROWS * LARGEST_LEADERBOARD_ROW_SIZE) +
    UINT8_WIRE_SIZE + LARGEST_LEADERBOARD_ROW_SIZE;

static_assert(LARGEST_LEADERBOARD_ANSWER_SIZE <= MESSAGE_PAYLOAD_LIMIT,
              "a LeaderboardAnswer always fits in one datagram");

/**
 * @brief The names of the fields a refused message is reported by, as the
 * protocol page writes them.
 */
constexpr std::string_view PLAYER_NUMBER_FIELD = "playerNumber";
constexpr std::string_view ENTITY_ID_FIELD = "entityId";
constexpr std::string_view ENTITY_TYPE_FIELD = "entityType";
constexpr std::string_view REASON_FIELD = "reason";
constexpr std::string_view OUTCOME_FIELD = "outcome";
constexpr std::string_view KIND_FIELD = "kind";
constexpr std::string_view ACTIONS_FIELD = "actions";
constexpr std::string_view ECHO_SENT_AT_FIELD = "echo.sentAt";
constexpr std::string_view ECHO_HELD_FOR_FIELD = "echo.heldFor";
constexpr std::string_view MOTION_FIELD = "motion";
constexpr std::string_view PLAYER_COUNT_FIELD = "playerCount";
constexpr std::string_view RANK_FIELD = "rank";
constexpr std::string_view PART_INDEX_FIELD = "partIndex";
constexpr std::string_view PART_COUNT_FIELD = "partCount";
constexpr std::string_view BASE_DISTANCE_FIELD = "baseDistance";

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
 * @brief The most ticks a DeltaWorldState reaches back to its reference: a
 * server never builds a delta on a state older than this, and a receiver
 * refuses a larger distance. 60 ticks are one second.
 */
constexpr std::uint8_t MAXIMUM_REFERENCE_DISTANCE = 60;

/**
 * @brief The baseDistance of a DeltaWorldState that has no reference: it lists
 * every entity, and a receiver needs no earlier state to read it.
 */
constexpr std::uint8_t NO_REFERENCE_DISTANCE = 0;

/**
 * @brief The most datagrams one DeltaWorldState is split into. A receiver
 * keeps the parts of a tick until the next tick comes, so this bounds what it
 * holds.
 */
constexpr std::uint8_t MAXIMUM_DELTA_PARTS = 16;

/**
 * @brief Bytes of a removed entity in a DeltaWorldState: its id.
 */
constexpr std::size_t REMOVED_ENTITY_WIRE_SIZE = UINT32_WIRE_SIZE;

/**
 * @brief Bytes of a DeltaWorldState that do not depend on its players,
 * changed and removed entities: tick, baseDistance, partIndex, partCount,
 * playerCount, changedCount and removedCount.
 */
constexpr std::size_t DELTA_WORLD_STATE_FIXED_SIZE =
    UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + UINT8_WIRE_SIZE + UINT8_WIRE_SIZE +
    UINT8_WIRE_SIZE + UINT8_WIRE_SIZE + UINT8_WIRE_SIZE;

/**
 * @brief The most changed or removed entities one DeltaWorldState holds: each
 * count is one byte.
 */
constexpr std::size_t MAXIMUM_ENTITIES_PER_DELTA_LIST =
    std::numeric_limits<std::uint8_t>::max();

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
