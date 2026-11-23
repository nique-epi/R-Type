#pragma once

#include <cstdint>
#include <limits>
#include <string_view>
#include "MasterRoute.hpp"
#include "RouteMethod.hpp"

namespace rtype::network {

/** @brief The path every route of the master starts with. */
constexpr std::string_view MASTER_API_PREFIX = "/api";

/** @brief Whether the master answers, and in which version. */
constexpr MasterRoute HEALTH_ROUTE{.method = RouteMethod::Get,
                                   .path = "/health"};

/**
 * @brief Creates a game server, named by its host on the web site; the
 * answer holds its key, the only time it is shown.
 */
constexpr MasterRoute CREATE_SERVER_ROUTE{.method = RouteMethod::Post,
                                          .path = "/servers"};

/** @brief Replaces the representation of the server of the key. */
constexpr MasterRoute REPLACE_OWN_SERVER_ROUTE{.method = RouteMethod::Put,
                                               .path = "/servers/me"};

/** @brief Replaces the last state of the server of the key. */
constexpr MasterRoute REPLACE_OWN_HEARTBEAT_ROUTE{
    .method = RouteMethod::Put, .path = "/servers/me/heartbeat"};

/** @brief The parameter of a path that names a server, after the prefix. */
constexpr std::string_view PATH_PARAMETER_PREFIX = ":";
constexpr std::string_view SERVER_ID_PATH_PARAMETER = "id";

/** @brief Lists the games of the server named in the path; open to everyone. */
constexpr MasterRoute LIST_SERVER_GAMES_ROUTE{.method = RouteMethod::Get,
                                              .path = "/servers/:id/games"};

/** @brief Lists the registered servers, a page at a time, by id; open to
 * everyone. */
constexpr MasterRoute LIST_SERVERS_ROUTE{.method = RouteMethod::Get,
                                         .path = "/servers"};

/** @brief Lists the samples of the server named in the path, a page at a
 * time, newest first; open to everyone. */
constexpr MasterRoute LIST_SERVER_SAMPLES_ROUTE{.method = RouteMethod::Get,
                                                .path = "/servers/:id/samples"};

/** @brief Lists the statuses the server named in the path held, each with the
 * second it took it, a page at a time, newest first; open to everyone. */
constexpr MasterRoute LIST_SERVER_STATUSES_ROUTE{
    .method = RouteMethod::Get, .path = "/servers/:id/statuses"};

/**
 * @brief Creates an account, named by its nickname, with its password. The
 * routes of the accounts are answered over HTTPS only.
 */
constexpr MasterRoute CREATE_USER_ROUTE{
    .method = RouteMethod::Post, .path = "/users", .httpsOnly = true};

/** @brief The account of the session of the token. */
constexpr MasterRoute READ_OWN_USER_ROUTE{
    .method = RouteMethod::Get, .path = "/users/me", .httpsOnly = true};

/**
 * @brief Opens a session for a nickname and its password; the answer holds
 * its token, the only time it is shown.
 */
constexpr MasterRoute CREATE_SESSION_ROUTE{
    .method = RouteMethod::Post, .path = "/sessions", .httpsOnly = true};

/** @brief Closes the session of the token; closing it again succeeds. */
constexpr MasterRoute DELETE_OWN_SESSION_ROUTE{
    .method = RouteMethod::Delete, .path = "/sessions/me", .httpsOnly = true};

/**
 * @brief Opens a device link for a game, which a player validates on the web
 * site; the answer holds its secret, the only time it is shown, and its short
 * code. The routes of the device links are answered over HTTPS only.
 */
constexpr MasterRoute CREATE_DEVICE_LINK_ROUTE{
    .method = RouteMethod::Post, .path = "/device-links", .httpsOnly = true};
/** @brief The device link of the secret the game sends as Bearer. */
constexpr MasterRoute READ_OWN_DEVICE_LINK_ROUTE{
    .method = RouteMethod::Get, .path = "/device-links/me", .httpsOnly = true};
/** @brief Forgets the device link of the secret; forgetting it again
 * succeeds. */
constexpr MasterRoute DELETE_OWN_DEVICE_LINK_ROUTE{
    .method = RouteMethod::Delete,
    .path = "/device-links/me",
    .httpsOnly = true};
/** @brief The parameter of a path that names a device link by its short
 * code. */
constexpr std::string_view DEVICE_LINK_CODE_PATH_PARAMETER = "code";
/** @brief The device link of the short code in the path, read by a signed-in
 * player on the web site before deciding. */
constexpr MasterRoute READ_DEVICE_LINK_ROUTE{.method = RouteMethod::Get,
                                             .path = "/device-links/:code",
                                             .httpsOnly = true};
/** @brief Replaces the decision on the device link of the short code in the
 * path, made by the account of the session: approved or denied, once. */
constexpr MasterRoute REPLACE_DEVICE_LINK_DECISION_ROUTE{
    .method = RouteMethod::Put,
    .path = "/device-links/:code/decision",
    .httpsOnly = true};
/**
 * @brief The balances of the wallet of the account of the token, opened at
 * its first read. The routes of the wallet are answered over HTTPS only, to
 * the account of a session.
 */
constexpr MasterRoute READ_OWN_WALLET_ROUTE{
    .method = RouteMethod::Get, .path = "/users/me/wallet", .httpsOnly = true};
/** @brief The journal of the wallet of the account of the token, a page at a
 * time, newest first. */
constexpr MasterRoute LIST_OWN_MOVEMENTS_ROUTE{
    .method = RouteMethod::Get,
    .path = "/users/me/wallet/movements",
    .httpsOnly = true};
/** @brief Changes kopecks into chips, or chips into kopecks, one for one, at
 * the cashier. */
constexpr MasterRoute CREATE_OWN_EXCHANGE_ROUTE{
    .method = RouteMethod::Post,
    .path = "/users/me/wallet/exchanges",
    .httpsOnly = true};
/** @brief The parameter of a path that names an exchange of the cashier. */
constexpr std::string_view EXCHANGE_ID_PATH_PARAMETER = "exchange";
/** @brief An exchange the cashier made for the account of the token. */
constexpr MasterRoute READ_OWN_EXCHANGE_ROUTE{
    .method = RouteMethod::Get,
    .path = "/users/me/wallet/exchanges/:exchange",
    .httpsOnly = true};
/** @brief Gives the account of the token a game ticket, which a game server
 * reports with the result of a game so the master pays that account. */
constexpr MasterRoute CREATE_OWN_TICKET_ROUTE{.method = RouteMethod::Post,
                                              .path = "/users/me/tickets",
                                              .httpsOnly = true};
/** @brief The parameter of a path that names a game ticket by its
 * fingerprint. */
constexpr std::string_view TICKET_PATH_PARAMETER = "ticket";
/** @brief A game ticket the master gave the account of the token, named by
 * its fingerprint. */
constexpr MasterRoute READ_OWN_TICKET_ROUTE{.method = RouteMethod::Get,
                                            .path = "/users/me/tickets/:ticket",
                                            .httpsOnly = true};
/** @brief The looks the account of the token owns. */
constexpr MasterRoute LIST_OWN_LOOKS_ROUTE{
    .method = RouteMethod::Get, .path = "/users/me/looks", .httpsOnly = true};
/** @brief The parameter of a path that names a look by its name. */
constexpr std::string_view LOOK_ITEM_PATH_PARAMETER = "item";
/**
 * @brief Makes the look named in the path one the account of the token owns,
 * buying it at its price; sent again, it buys nothing more.
 */
constexpr MasterRoute BUY_OWN_LOOK_ROUTE{.method = RouteMethod::Put,
                                         .path = "/users/me/looks/:item",
                                         .httpsOnly = true};

/** @brief The parameters of a path that name a room of the casino and a
 * round of its wheel. */
constexpr std::string_view ROOM_PATH_PARAMETER = "room";
constexpr std::string_view ROUND_PATH_PARAMETER = "round";
/** @brief The roulette rooms of the casino of the master, with their round
 * and phase; open to everyone, over plain HTTP too. */
constexpr MasterRoute LIST_CASINO_ROOMS_ROUTE{.method = RouteMethod::Get,
                                              .path = "/casino/rooms"};
/** @brief The state of one room: its round, the dates of its phases, the
 * number drawn once the bets closed, the last numbers and the players seated;
 * open to everyone. */
constexpr MasterRoute READ_CASINO_ROOM_ROUTE{.method = RouteMethod::Get,
                                             .path = "/casino/rooms/:room"};
/**
 * @brief Seats the account of the token in the room, or renews its seat: a
 * seat not renewed for CASINO_SEAT_LIFETIME is lost. The routes of the seats
 * and of the bets are answered over HTTPS only, to the account of a session.
 */
constexpr MasterRoute TAKE_OWN_SEAT_ROUTE{
    .method = RouteMethod::Put,
    .path = "/casino/rooms/:room/seats/me",
    .httpsOnly = true};
/** @brief Gets the account of the token up from the room; leaving again
 * succeeds. */
constexpr MasterRoute LEAVE_OWN_SEAT_ROUTE{
    .method = RouteMethod::Delete,
    .path = "/casino/rooms/:room/seats/me",
    .httpsOnly = true};
/** @brief Replaces every bet the account of the token holds on a round of a
 * room, while its bets are open: the bets it held are handed back, the new
 * ones staked. */
constexpr MasterRoute PLACE_OWN_BETS_ROUTE{
    .method = RouteMethod::Put,
    .path = "/casino/rooms/:room/rounds/:round/bets/me",
    .httpsOnly = true};
/** @brief The bets the account of the token holds on a round of a room, and
 * what the round paid them once drawn. */
constexpr MasterRoute READ_OWN_BETS_ROUTE{
    .method = RouteMethod::Get,
    .path = "/casino/rooms/:room/rounds/:round/bets/me",
    .httpsOnly = true};

/** @brief The parameter of a path that names a game of a server. */
constexpr std::string_view GAME_ID_PATH_PARAMETER = "gameId";

/**
 * @brief Keeps the result of a vanilla game of the server of the key, named in
 * the path by the id the server drew; a game the master already keeps stays
 * as it was, so a server may send a result again safely.
 */
constexpr MasterRoute KEEP_OWN_RESULT_ROUTE{
    .method = RouteMethod::Put, .path = "/servers/me/results/:gameId"};

/**
 * @brief The best score of each player for one difficulty and one number of
 * players, on every server or on one, a page at a time, with the row of a
 * player the query names; open to everyone.
 */
constexpr MasterRoute READ_LEADERBOARD_ROUTE{.method = RouteMethod::Get,
                                             .path = "/leaderboard"};

/**
 * @brief The accounts by the total of the scores of the games credited to
 * each, every difficulty, number of players and server together, a page at a
 * time; open to everyone.
 */
constexpr MasterRoute READ_ACCOUNT_LEADERBOARD_ROUTE{
    .method = RouteMethod::Get, .path = "/leaderboard/accounts"};

/** @brief The parameters of the query of the leaderboard:
 * `?difficulty=normal&playerCount=2`, then `server=7` and `player=ACE`, both
 * optional. */
constexpr std::string_view DIFFICULTY_QUERY_PARAMETER = "difficulty";
constexpr std::string_view PLAYER_COUNT_QUERY_PARAMETER = "playerCount";
constexpr std::string_view SERVER_QUERY_PARAMETER = "server";
constexpr std::string_view PLAYER_QUERY_PARAMETER = "player";

/**
 * @brief The parameters of the query of a list, `?limit=50&offset=0`, and
 * their bounds: a page holds MINIMUM_PAGE_SIZE to MAXIMUM_PAGE_SIZE items,
 * DEFAULT_PAGE_SIZE when the query leaves limit out, and starts after the
 * first offset items, FIRST_PAGE_OFFSET when it leaves offset out.
 */
constexpr std::string_view LIMIT_QUERY_PARAMETER = "limit";
constexpr std::string_view OFFSET_QUERY_PARAMETER = "offset";
constexpr std::int64_t MINIMUM_PAGE_SIZE = 1;
constexpr std::int64_t DEFAULT_PAGE_SIZE = 50;
constexpr std::int64_t MAXIMUM_PAGE_SIZE = 100;
constexpr std::int64_t FIRST_PAGE_OFFSET = 0;
constexpr std::int64_t HIGHEST_PAGE_OFFSET =
    std::numeric_limits<std::int64_t>::max();

/** @brief What joins the path of the servers and the id of one of them. */
constexpr std::string_view PATH_SEPARATOR = "/";

}  // namespace rtype::network
