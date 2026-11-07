#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client {

/**
 * @brief Time between two ReadyState while the LobbyState of the server shows
 * another value than the one the player asked for. Provisional, see the
 * protocol page.
 */
constexpr std::chrono::milliseconds READY_STATE_INTERVAL{500};

/**
 * @brief Time between two LeaderboardRequest while no answer came, the time
 * after which a server answers the same player again; and how long the client
 * asks before it gives up. As the protocol page writes them.
 */
constexpr std::chrono::milliseconds LEADERBOARD_ASK_INTERVAL{500};
constexpr std::chrono::seconds LEADERBOARD_GIVE_UP_TIME{5};

/**
 * @brief Time between two GameLoaded while the game is started and no
 * WorldState arrived yet. Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds GAME_LOADED_INTERVAL{100};

/**
 * @brief How many times a Disconnection goes out when the player leaves a
 * server, and the time between two copies: nothing repeats it, so a lost copy
 * is made up by the next. Provisional, see the protocol page.
 */
constexpr int DISCONNECTION_COPY_COUNT = 3;
constexpr std::chrono::milliseconds DISCONNECTION_COPY_INTERVAL{50};

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

/** @brief Logger name the connection to a server writes its log lines under. */
constexpr std::string_view CONNECTION_LOGGER_NAME = "Connection";

}  // namespace rtype::client
