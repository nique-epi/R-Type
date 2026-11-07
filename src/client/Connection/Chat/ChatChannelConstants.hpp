#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/**
 * @brief A line of chat: sent again every CHAT_LINE_INTERVAL until the server
 * relays it back, given up after CHAT_GIVE_UP_TIME (the protocol page, Chat);
 * the number of the first line of a session; the lines that may wait behind
 * the one sent; the lines the client keeps, the oldest dropped first.
 */
constexpr std::chrono::milliseconds CHAT_LINE_INTERVAL{500};
constexpr std::chrono::seconds CHAT_GIVE_UP_TIME{5};
constexpr std::uint16_t FIRST_CHAT_LINE_NUMBER = 1;
constexpr std::size_t CHAT_LINES_WAITING = 3;
constexpr std::size_t CHAT_LINES_KEPT = 32;

/** @brief The player number of a line the client writes itself, and what
 * those lines say: a line the server never relayed back, a line refused while
 * CHAT_LINES_WAITING lines wait. */
constexpr std::uint8_t CLIENT_WRITER = 0;
constexpr std::string_view UNANSWERED_CHAT_TEXT = "YOUR LINE GOT NO ANSWER";
constexpr std::string_view CHAT_QUEUE_FULL_TEXT = "TOO MANY LINES: WAIT";

}  // namespace rtype::client
