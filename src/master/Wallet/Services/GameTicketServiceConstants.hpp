#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::master {

/**
 * @brief How long a game ticket pays: a result the master receives after it
 * pays nothing. The client asks one right before it joins, and a game of the
 * level lasts minutes, so 6 h leaves a server that lost the master hours to
 * report the game.
 */
constexpr std::chrono::seconds GAME_TICKET_LIFETIME = std::chrono::hours{6};

/**
 * @brief How many tickets one account may be given in GAME_TICKET_WINDOW: a
 * player asks one per game it joins, and each is a row until it expires, so
 * the quota bounds the rows a session can make the master keep.
 */
constexpr std::int64_t MAXIMUM_GAME_TICKETS_PER_WINDOW = 30;
constexpr std::chrono::seconds GAME_TICKET_WINDOW = std::chrono::hours{1};

}  // namespace rtype::master
