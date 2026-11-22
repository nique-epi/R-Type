#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "Translation.hpp"

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
constexpr Translation UNANSWERED_CHAT_TEXT{
    .english = "YOUR LINE GOT NO ANSWER",
    .french = "VOTRE LIGNE EST RESTÉE SANS RÉPONSE"};
constexpr Translation CHAT_QUEUE_FULL_TEXT{
    .english = "TOO MANY LINES: WAIT", .french = "TROP DE LIGNES : ATTENDEZ"};

}  // namespace rtype::client
