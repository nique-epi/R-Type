#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief Texts of 32 and 33 bytes: the longest server name and game version
 * the protocol accepts, and one byte more. */
constexpr std::string_view THIRTY_TWO_BYTE_TEXT =
    "abcdefghijklmnopqrstuvwxyz012345";
constexpr std::string_view THIRTY_THREE_BYTE_TEXT =
    "abcdefghijklmnopqrstuvwxyz0123456";

/** @brief A server name holding a line feed. */
constexpr std::string_view SERVER_NAME_WITH_LINE_FEED = "R-Type\nserver";

/** @brief An answer of two games, which hold eight seats. */
constexpr std::uint32_t ANSWERED_SENT_AT = 0x01020304;
constexpr std::uint32_t SOME_HELD_FOR = 0x0A0B0C0D;
constexpr std::uint8_t SOME_PROTOCOL_VERSION = 3;
constexpr std::string_view SOME_GAME_VERSION = "0.1.0";
constexpr std::string_view SOME_SERVER_NAME = "Lyon";
constexpr std::uint16_t TWO_GAMES = 2;
constexpr std::uint16_t SEATS_OF_TWO_GAMES = 8;
constexpr std::uint16_t FIVE_PLAYERS = 5;
constexpr std::uint16_t THREE_FREE_SEATS = 3;

/** @brief The largest datagram of an answer, added up by hand from the
 * protocol page: 22 + 3 + 4 + 4 + 1 + 33 + 33 + 2 + 2 + 2. */
constexpr std::size_t LARGEST_ANSWER_DATAGRAM_ON_THE_PROTOCOL_PAGE = 106;

}  // namespace rtype::network::testing
