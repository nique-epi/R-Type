#pragma once

#include <chrono>
#include <string_view>

namespace rtype::client {

/**
 * @brief Time between two ConnectionRequest of an attempt while the server
 * has not answered. Provisional, see the protocol page.
 */
constexpr std::chrono::milliseconds CONNECTION_REQUEST_INTERVAL{500};

/**
 * @brief Time after the start of an attempt at which it gives up, when the
 * server has neither accepted nor refused it. Provisional, see the protocol
 * page.
 */
constexpr std::chrono::seconds CONNECTION_ATTEMPT_TIMEOUT{5};

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

/** @brief Logger name the connection to a server writes its log lines under. */
constexpr std::string_view CONNECTION_LOGGER_NAME = "Connection";

}  // namespace rtype::client
