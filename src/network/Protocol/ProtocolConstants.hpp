#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "ContentPackConstants.hpp"
#include "EntityTypeConstants.hpp"
#include "NetworkConstants.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network {

constexpr std::size_t INT16_WIRE_SIZE = sizeof(std::int16_t);

/**
 * @brief The first four bytes of every datagram, "RTYP" in ASCII.
 */
constexpr std::uint32_t PROTOCOL_ID = 0x52545950;

/**
 * @brief The version of the protocol this code speaks. Version 15 removes
 * the ban: the reason Banned of ConnectionRefused (9), Expulsion (2) and
 * PlayerLeft (4) is invalid again, see the Version 15 section; version 14
 * added it; version 13 adds the expulsion of a player: ExpulsionOrder from the
 * supervisor to a game, Expulsion to the player and the departure reason
 * Expelled, see the Version 13 section; version 12 makes a ConnectionRequest
 * for a NewGame carry the rules of that game, and adds the refusals
 * UnknownDifficulty and LivesOutOfRange, see the Version 12 section; version 11
 * makes EntitySpawned name the player and the input that fired a missile, and
 * PlayerInput the tick its client shows, so a client can predict its own
 * missiles and the server test them where the player saw the enemies, see
 * the Version 11 section; version 10 makes a ConnectionAccepted announce the
 * content pack of the game in place of the table of its entity types, see
 * the Version 10 section; version 9 makes a
 * ConnectionRequest name the game it asks for, and adds the refusals
 * GameNotFound and ServerFull, see the Version 9 section; version 8 makes a
 * GameStatus carry the nicknames of the players of its game and the measures
 * of its process, see the Version 8 section; version 7 numbers the
 * guaranteed messages and delivers them in order, see the Ordered messages
 * section; version 6 names the
 * entity types by a table ConnectionAccepted carries, so the type of
 * EntitySpawned, WorldState and DeltaWorldState is a number of that table,
 * see the Version 6 section of the protocol page; version 5 makes a
 * PlayerInput repeat the inputs the server has not confirmed, see the Inputs
 * section; version 4 lets the server send the body of a datagram as a zstd
 * frame, see the Compression section; version 3 adds the outcome of a game to
 * GameEnded, see the Version 3 section.
 */
constexpr std::uint8_t PROTOCOL_VERSION = 15;

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
 * @brief The flag of a datagram whose body, the bytes after the header, is a
 * zstd frame. Only the transport sees it: the socket that receives the
 * datagram decodes the body and clears the flag before anything else reads it,
 * so it is not one of KNOWN_FLAGS and a header that still holds it is refused.
 */
constexpr std::uint8_t COMPRESSED_FLAG = 0x04;

/**
 * @brief Where the flags byte sits in a datagram: after the protocol id and
 * the version.
 */
constexpr std::size_t FLAGS_OFFSET = UINT32_WIRE_SIZE + UINT8_WIRE_SIZE;

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
 * @brief Where each measure of a GameStatus sits in its measures byte, counted
 * from bit 0: the median of the duration of a tick, its 95th percentile, and
 * the mean round trip of the players of the game. A measure whose bit is set
 * follows, in that order, as a 32-bit count of microseconds; the bits above
 * them are 0.
 */
constexpr std::uint8_t TICK_MEDIAN_MEASURE_BIT_POSITION = 0;
constexpr std::uint8_t TICK_NINETY_FIFTH_PERCENTILE_MEASURE_BIT_POSITION = 1;
constexpr std::uint8_t AVERAGE_ROUND_TRIP_MEASURE_BIT_POSITION = 2;
constexpr std::size_t GAME_STATUS_MEASURE_COUNT = 3;
constexpr auto GAME_STATUS_MEASURE_BITS =
    static_cast<std::uint8_t>((1U << GAME_STATUS_MEASURE_COUNT) - 1);

/**
 * @brief The inputNumber of the first PlayerInput of a session; each next one
 * is one more.
 */
constexpr std::uint32_t FIRST_INPUT_NUMBER = 1;

static_assert(identifiers::MAXIMUM_ENTITY_TYPES <=
                  std::numeric_limits<std::uint8_t>::max(),
              "the number of a type is one byte, and 0 names none");

/**
 * @brief The most inputs before the newest that one PlayerInput repeats. At
 * 60 inputs per second it covers 250 ms: the wait of the 50 ms between two
 * WorldState that confirm inputs, plus a round trip. Provisional, see the
 * protocol page.
 */
constexpr std::size_t MAXIMUM_REPEATED_INPUTS = 15;

/**
 * @brief PlayerInput a client sends per second during a game. The server
 * derives the rate limit of a player from it. Provisional, see the protocol
 * page.
 */
constexpr std::size_t PLAYER_INPUTS_PER_SECOND = 60;

/**
 * @brief Time between two ConnectionRequest of an attempt while the server
 * has not answered. The server derives the rate limit of the requests from
 * it, and paces the acceptance it repeats on it. Provisional, see the
 * protocol page.
 */
constexpr std::chrono::milliseconds CONNECTION_REQUEST_INTERVAL{500};

/**
 * @brief Time after the start of an attempt at which the client gives up,
 * when the server has neither accepted nor refused it: it bounds the requests
 * one attempt sends. Provisional, see the protocol page.
 */
constexpr std::chrono::seconds CONNECTION_ATTEMPT_TIMEOUT{5};

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
 * @brief The number of the first message of the ordered channel of a session;
 * each next one is one more, wrapping from 65535 to 0.
 */
constexpr std::uint16_t FIRST_ORDERED_NUMBER = 1;

/**
 * @brief How many numbers the ordered channel spans at most: a sender gives up
 * a message whose number is this far behind the newest, so a receiver holds at
 * most this many messages.
 */
constexpr std::size_t ORDERED_WINDOW = 256;

static_assert(std::numeric_limits<std::uint16_t>::max() % ORDERED_WINDOW ==
                  ORDERED_WINDOW - 1,
              "a number maps to a slot by its remainder, across the wrap");
static_assert(ORDERED_WINDOW < std::numeric_limits<std::uint16_t>::max() / 2,
              "numbers are compared with serial number arithmetic");

/**
 * @brief Bytes the envelope of an ordered message adds to the message it
 * carries: its number, firstKept and the type of the carried message.
 */
constexpr std::size_t ORDERED_PREFIX_SIZE =
    UINT16_WIRE_SIZE + UINT16_WIRE_SIZE + UINT8_WIRE_SIZE;

/**
 * @brief The most bytes the payload of a message carried by the ordered
 * channel holds.
 */
constexpr std::size_t ORDERED_PAYLOAD_LIMIT =
    MESSAGE_PAYLOAD_LIMIT - ORDERED_PREFIX_SIZE;

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
 * @brief The most difficulties a server may hold, so its heartbeat and the
 * list of its games stay small: one per table of its waves file, the shipped
 * file having three.
 */
constexpr std::size_t MAXIMUM_DIFFICULTIES = 16;

/**
 * @brief The longest name of a server, in bytes of UTF-8, the master and the
 * ServerInformation of the server alike.
 */
constexpr std::size_t MAXIMUM_SERVER_NAME_SIZE = 32;

/**
 * @brief The longest text of a chat line, in bytes of UTF-8: Source cuts a line
 * of chat at 127 characters. Provisional, see the protocol page.
 */
constexpr std::size_t MAXIMUM_CHAT_TEXT_SIZE = 127;

static_assert(MAXIMUM_CHAT_TEXT_SIZE <= MAX_SHORT_STRING_LENGTH,
              "a chat text fits a short string");

/**
 * @brief The longest version of the game, in bytes: "1.12.0-rc.2" fits. The
 * master and the ServerInformation of the server alike.
 */
constexpr std::size_t MAXIMUM_GAME_VERSION_SIZE = 32;

/**
 * @brief The control characters a nickname may not hold are the bytes below
 * SPACE_BYTE, U+0000 to U+001F, and DELETE_BYTE, U+007F. In UTF-8 each of them
 * is one byte of its own value, and every byte of a longer sequence is 0x80 or
 * above, so reading the bytes finds exactly these characters.
 */
constexpr std::uint8_t SPACE_BYTE = 0x20;
constexpr std::uint8_t DELETE_BYTE = 0x7F;

/**
 * @brief The number of the first game of a server, in the list of the master
 * and in a ConnectionRequest that names a game; its games are numbered from it.
 */
constexpr std::uint16_t FIRST_HOSTED_GAME_NUMBER = 1;

/**
 * @brief The number a ConnectionRequest carries when its choice names no game:
 * any game, or a new one.
 */
constexpr std::uint16_t UNUSED_GAME_NUMBER = 0;

/**
 * @brief The size of the smallest datagram holding a ConnectionRequest: a
 * cookie, a nickname of one byte, then the kind and the number of the game.
 */
constexpr std::size_t SMALLEST_CONNECTION_REQUEST_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT64_WIRE_SIZE +
    SHORT_STRING_PREFIX_SIZE + MINIMUM_NICKNAME_SIZE + UINT8_WIRE_SIZE +
    UINT16_WIRE_SIZE;

/**
 * @brief The size of the largest datagram the server sends a sender that did
 * not prove its address, that is a request without a valid cookie: a
 * ConnectionChallenge and its cookie. ConnectionRefused carries one byte.
 * ConnectionAccepted answers only a request with a valid cookie, which only
 * the owner of the address could send back.
 */
constexpr std::size_t LARGEST_UNPROVEN_ANSWER_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT64_WIRE_SIZE;

static_assert(LARGEST_UNPROVEN_ANSWER_DATAGRAM_SIZE <=
                  SMALLEST_CONNECTION_REQUEST_DATAGRAM_SIZE,
              "the server never sends a sender that did not prove its "
              "address more bytes than the request it answers");

/**
 * @brief The size of a datagram holding a ConnectionRedirect and its port: the
 * supervisor answers a request with a valid cookie with it, so it is never
 * larger than the request it answers, like every answer to a sender.
 */
constexpr std::size_t CONNECTION_REDIRECT_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT16_WIRE_SIZE;

static_assert(CONNECTION_REDIRECT_DATAGRAM_SIZE <=
                  SMALLEST_CONNECTION_REQUEST_DATAGRAM_SIZE,
              "the supervisor never sends a sender more bytes than the "
              "request it answers");

/**
 * @brief The size of the payload of a ConnectionAccepted: the player number,
 * then the fingerprint, the size and the port of the content pack.
 */
constexpr std::size_t CONNECTION_ACCEPTED_PAYLOAD_SIZE =
    UINT8_WIRE_SIZE + CONTENT_FINGERPRINT_BYTES + UINT32_WIRE_SIZE +
    UINT16_WIRE_SIZE;

static_assert(PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE +
                      CONNECTION_ACCEPTED_PAYLOAD_SIZE <=
                  MAX_DATAGRAM_SIZE,
              "a ConnectionAccepted always fits in one datagram");

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
 * @brief The values of a LeaderboardRequest that ask for every number of
 * players and every difficulty: a number of players below FIRST_PLAYER_NUMBER
 * and a difficulty shorter than any name.
 */
constexpr std::uint8_t ALL_PLAYER_COUNTS = 0;
constexpr std::string_view ALL_DIFFICULTIES;

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
 * @brief The largest PlayerChatLine payload: lineNumber, the longest text after
 * its length, then playerNumber.
 */
constexpr std::size_t LARGEST_PLAYER_CHAT_LINE_SIZE =
    UINT16_WIRE_SIZE + SHORT_STRING_PREFIX_SIZE + MAXIMUM_CHAT_TEXT_SIZE +
    UINT8_WIRE_SIZE;

static_assert(LARGEST_PLAYER_CHAT_LINE_SIZE <= ORDERED_PAYLOAD_LIMIT,
              "a PlayerChatLine always fits in an OrderedMessage");

/**
 * @brief The size of a ship look on the wire: head, eyes, face, one byte each.
 */
constexpr std::size_t SHIP_LOOK_WIRE_SIZE = 3 * UINT8_WIRE_SIZE;

/**
 * @brief The largest PlayerLooks payload: the count, then a player number and
 * a ship look for each of MAXIMUM_PLAYERS players, 1 + 4 x 4 = 17 bytes.
 */
constexpr std::size_t LARGEST_PLAYER_LOOKS_SIZE =
    UINT8_WIRE_SIZE +
    (MAXIMUM_PLAYERS * (UINT8_WIRE_SIZE + SHIP_LOOK_WIRE_SIZE));

static_assert(LARGEST_PLAYER_LOOKS_SIZE <= ORDERED_PAYLOAD_LIMIT,
              "a PlayerLooks always fits in an OrderedMessage");

/**
 * @brief The size of the largest datagram holding a ServerInformation: the
 * echo of the request (sentAt, heldFor), the protocol version, the longest
 * game version and server name, then the counts of players, games and free
 * seats. 106 bytes.
 */
constexpr std::size_t LARGEST_SERVER_INFORMATION_DATAGRAM_SIZE =
    PACKET_HEADER_SIZE + MESSAGE_PREFIX_SIZE + UINT32_WIRE_SIZE +
    UINT32_WIRE_SIZE + UINT8_WIRE_SIZE + SHORT_STRING_PREFIX_SIZE +
    MAXIMUM_GAME_VERSION_SIZE + SHORT_STRING_PREFIX_SIZE +
    MAXIMUM_SERVER_NAME_SIZE + UINT16_WIRE_SIZE + UINT16_WIRE_SIZE +
    UINT16_WIRE_SIZE;

/**
 * @brief The size of every datagram holding a ServerInformationRequest: the
 * size of the largest answer, by definition, so the server never sends a
 * sender it does not know more bytes than it received, as QUIC pads its first
 * datagram (RFC 9000, section 8.1).
 */
constexpr std::size_t SERVER_INFORMATION_REQUEST_DATAGRAM_SIZE =
    LARGEST_SERVER_INFORMATION_DATAGRAM_SIZE;

/**
 * @brief The zero bytes a ServerInformationRequest carries after its sentAt,
 * so its datagram is SERVER_INFORMATION_REQUEST_DATAGRAM_SIZE long. 77 bytes.
 */
constexpr std::size_t SERVER_INFORMATION_REQUEST_PADDING_SIZE =
    SERVER_INFORMATION_REQUEST_DATAGRAM_SIZE - PACKET_HEADER_SIZE -
    MESSAGE_PREFIX_SIZE - UINT32_WIRE_SIZE;

/**
 * @brief The value of every byte of the padding of a ServerInformationRequest.
 */
constexpr std::uint8_t PADDING_BYTE = 0;

/** @brief The owner of an EntitySpawned that no player fired. */
constexpr std::uint8_t NO_OWNER = 0;

/** @brief The firingInput of an EntitySpawned that no input fired. */
constexpr std::uint32_t NO_FIRING_INPUT = 0;

/** @brief The shownTick of a PlayerInput sent before the first state. */
constexpr std::uint32_t NO_SHOWN_TICK = 0;

/**
 * @brief The names of the fields a refused message is reported by, as the
 * protocol page writes them.
 */
constexpr std::string_view PLAYER_NUMBER_FIELD = "playerNumber";
constexpr std::string_view HEAD_FIELD = "head";
constexpr std::string_view EYES_FIELD = "eyes";
constexpr std::string_view FACE_FIELD = "face";
constexpr std::string_view ITEM_FIELD = "item";
constexpr std::string_view ENTITY_ID_FIELD = "entityId";
constexpr std::string_view ENTITY_TYPE_FIELD = "entityType";
constexpr std::string_view REASON_FIELD = "reason";
constexpr std::string_view OUTCOME_FIELD = "outcome";
constexpr std::string_view KIND_FIELD = "kind";
constexpr std::string_view ACTIONS_FIELD = "actions";
constexpr std::string_view EARLIER_COUNT_FIELD = "earlierCount";
constexpr std::string_view ECHO_SENT_AT_FIELD = "echo.sentAt";
constexpr std::string_view ECHO_HELD_FOR_FIELD = "echo.heldFor";
constexpr std::string_view TICK_FIELD = "tick";
constexpr std::string_view MOTION_FIELD = "motion";
constexpr std::string_view PLAYER_COUNT_FIELD = "playerCount";
constexpr std::string_view RANK_FIELD = "rank";
constexpr std::string_view PART_INDEX_FIELD = "partIndex";
constexpr std::string_view PART_COUNT_FIELD = "partCount";
constexpr std::string_view BASE_DISTANCE_FIELD = "baseDistance";
constexpr std::string_view CONTENT_SIZE_FIELD = "content.size";
constexpr std::string_view OWNER_FIELD = "owner";
constexpr std::string_view NUMBER_FIELD = "number";
constexpr std::string_view RULES_FIELD = "rules";
constexpr std::string_view FIRST_KEPT_FIELD = "firstKept";
constexpr std::string_view INNER_TYPE_FIELD = "innerType";
constexpr std::string_view CARRIED_PAYLOAD_SIZE_FIELD = "carriedPayloadSize";
constexpr std::string_view PHASE_FIELD = "phase";
constexpr std::string_view PORT_FIELD = "port";
constexpr std::string_view STATE_FIELD = "state";
constexpr std::string_view PADDING_FIELD = "padding";
constexpr std::string_view FREE_SEAT_COUNT_FIELD = "freeSeatCount";
constexpr std::string_view GAME_VERSION_FIELD = "gameVersion";
constexpr std::string_view SERVER_NAME_FIELD = "name";
constexpr std::string_view MEASURES_FIELD = "measures";
constexpr std::string_view BOSS_ENTITY_ID_FIELD = "bossEntityId";
constexpr std::string_view HEALTH_FIELD = "health";
constexpr std::string_view FULL_HEALTH_FIELD = "fullHealth";
constexpr std::string_view STAGE_FIELD = "stage";
constexpr std::string_view BIOME_FIELD = "biome";
constexpr std::string_view SCROLL_POSITION_FIELD = "scrollPosition";
constexpr std::string_view SCROLL_SPEED_FIELD = "scrollSpeed";

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
 * @brief The smallest and the largest count of eighths of a unit a wire
 * coordinate holds, that is -4096 and 4095.875 units.
 */
constexpr float MINIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::min());
constexpr float MAXIMUM_WIRE_STEPS =
    static_cast<float>(std::numeric_limits<std::int16_t>::max());

}  // namespace rtype::network
