#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/**
 * @brief A coordinate of minus one logical unit, which the wire carries as -8
 * eighths.
 */
constexpr float MINUS_ONE_UNIT = -1.0F;

/**
 * @brief Nicknames of 1 and 16 bytes, the ends of the sizes the protocol
 * accepts, and one of 17 bytes.
 */
constexpr std::string_view ONE_BYTE_NICKNAME = "A";
constexpr std::string_view SIXTEEN_BYTE_NICKNAME = "abcdefghijklmnop";
constexpr std::string_view SEVENTEEN_BYTE_NICKNAME = "abcdefghijklmnopq";

/**
 * @brief Four rockets, U+1F680, of four bytes each: sixteen bytes.
 */
constexpr std::string_view FOUR_ROCKETS =
    "\xF0\x9F\x9A\x80\xF0\x9F\x9A\x80\xF0\x9F\x9A\x80\xF0\x9F\x9A\x80";

/**
 * @brief The letter e with an acute accent, U+00E9, in two bytes.
 */
constexpr std::string_view ACCENTED_LETTER = "\xC3\xA9";

/**
 * @brief Well-formed UTF-8 at the edges of the table of RFC 3629: the last
 * one-byte character, the first and last two-byte ones, the first three-byte
 * one, the last before the surrogates, the first after them, the first
 * four-byte one and U+10FFFF, the last code point.
 */
constexpr auto WELL_FORMED_UTF8_AT_THE_EDGES = std::to_array<std::string_view>({
    "\x7F",
    "\xC2\x80",
    "\xDF\xBF",
    "\xE0\xA0\x80",
    "\xED\x9F\xBF",
    "\xEE\x80\x80",
    "\xF0\x90\x80\x80",
    "\xF4\x8F\xBF\xBF",
});

/**
 * @brief Ill-formed UTF-8: an overlong two-byte and three-byte form, a
 * surrogate, a code point above U+10FFFF, a byte no sequence starts with, a
 * stray continuation byte, a sequence cut short, and a three-byte and a
 * four-byte sequence whose last byte is a letter instead of a continuation
 * byte.
 */
constexpr auto ILL_FORMED_UTF8 = std::to_array<std::string_view>({
    "\xC0\x80",
    "\xE0\x80\x80",
    "\xED\xA0\x80",
    "\xF4\x90\x80\x80",
    "\xF5\x80\x80\x80",
    "\x80",
    "\xE2\x82",
    "\xE1\x80\x41",
    "\xF0\x90\x80\x41",
});

/**
 * @brief A session token and a sequence whose bytes all differ, so a test sees
 * each byte at its offset.
 */
constexpr std::uint64_t SOME_SESSION_TOKEN = 0x0102030405060708;
constexpr std::uint16_t SOME_SEQUENCE = 0x0A0B;

/**
 * @brief Values for the header fields version 0 leaves at 0, all different,
 * so a round trip shows each one at its place.
 */
constexpr std::uint8_t SOME_OTHER_VERSION = 1;
constexpr std::uint32_t SOME_OTHER_PROTOCOL_ID = 0x52545951;
constexpr std::uint8_t SOME_FLAGS = 0x0C;
constexpr std::uint16_t SOME_ACKNOWLEDGED_SEQUENCE = 0x0D0E;
constexpr std::uint32_t SOME_ACKNOWLEDGEMENT_BITS = 0x0F101112;

/**
 * @brief The 22 bytes of a default header: RTYP, version 0, then zeros.
 */
constexpr auto DEFAULT_HEADER_BYTES = std::to_array<std::uint8_t>(
    {0x52, 0x54, 0x59, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});

/**
 * @brief The offset of the version byte in a datagram.
 */
constexpr std::size_t VERSION_OFFSET = 4;

/**
 * @brief The size of a datagram holding one message with an empty payload:
 * the 22-byte header and the 3-byte message prefix.
 */
constexpr std::size_t EMPTY_MESSAGE_DATAGRAM_SIZE = 25;

/**
 * @brief The size of every datagram a version 0 sender may send at most.
 */
constexpr std::size_t LARGEST_DATAGRAM_SIZE = 1200;

/**
 * @brief A cookie whose bytes all differ, so a test sees each at its offset.
 */
constexpr std::uint64_t SOME_COOKIE = 0x0102030405060708;

/**
 * @brief The last player number of a game, and the numbers just outside the
 * range 1 to 4.
 */
constexpr std::uint8_t LAST_PLAYER_NUMBER = 4;
constexpr std::uint8_t PLAYER_NUMBER_BELOW_THE_FIRST = 0;
constexpr std::uint8_t PLAYER_NUMBER_ABOVE_THE_LAST = 5;

/**
 * @brief Refusal reasons just outside the values 1 to 3 of the protocol page.
 */
constexpr std::uint8_t REFUSAL_REASON_BELOW_THE_FIRST = 0;
constexpr std::uint8_t REFUSAL_REASON_ABOVE_THE_LAST = 4;

/**
 * @brief The sizes the protocol page gives for the smallest ConnectionRequest
 * datagram and for a ConnectionChallenge datagram.
 */
constexpr std::size_t SMALLEST_REQUEST_DATAGRAM_SIZE_ON_THE_PAGE = 35;
constexpr std::size_t CHALLENGE_DATAGRAM_SIZE_ON_THE_PAGE = 33;

/**
 * @brief A score whose bytes all differ, so a test sees each at its offset.
 */
constexpr std::uint32_t SOME_SCORE_OF_DISTINCT_BYTES = 0x01020304;

/**
 * @brief The size of the largest LobbyState datagram: the header, the prefix,
 * the player count, and four players of 1 + 1 + 1 + 16 bytes.
 */
constexpr std::size_t LARGEST_LOBBY_DATAGRAM_SIZE = 102;

/**
 * @brief An input number and an event number whose bytes all differ.
 */
constexpr std::uint32_t SOME_INPUT_NUMBER = 0x01020304;
constexpr std::uint16_t SOME_EVENT_NUMBER = 0x0102;

/**
 * @brief Actions of a PlayerInput: up and shoot (bits 0 and 4), and all five
 * actions at once.
 */
constexpr std::uint8_t UP_AND_SHOOT = 0x11;
constexpr std::uint8_t EVERY_ACTION = 0x1F;

/**
 * @brief The first bit above the five actions, which must stay 0.
 */
constexpr std::uint8_t FIRST_BIT_ABOVE_THE_ACTIONS = 0x20;

/**
 * @brief The number of a player who is not the first, for messages naming
 * one.
 */
constexpr std::uint8_t SECOND_PLAYER_NUMBER = 2;

/**
 * @brief The port a test message is said to come from.
 */
constexpr std::uint16_t SOME_SENDER_PORT = 50000;

/**
 * @brief Message type bytes no message has: the byte before the first type,
 * the byte after the last, and the largest byte.
 */
constexpr std::uint8_t TYPE_BELOW_THE_FIRST = 0;
constexpr std::uint8_t TYPE_ABOVE_THE_LAST = 17;
constexpr std::uint8_t LARGEST_TYPE_BYTE = 255;

/**
 * @brief How many messages a test has a dispatcher reject at once, to check
 * that they share one log line.
 */
constexpr std::size_t REJECTED_MESSAGE_COUNT = 3;

/**
 * @brief Words of every log line about a message a dispatcher rejects, which
 * tell those lines apart from the others.
 */
constexpr std::string_view REJECTED_MESSAGE_LINE_TEXT = "a message of type";

}  // namespace rtype::network::testing
