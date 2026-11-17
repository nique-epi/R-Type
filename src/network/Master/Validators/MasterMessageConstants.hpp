#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include "NetworkConstants.hpp"
#include "ShipLookConstants.hpp"

namespace rtype::network {

/** @brief The members of the registration of a server, in JSON. */
constexpr std::string_view NAME_MEMBER = "name";
constexpr std::string_view ADDRESS_MEMBER = "address";
constexpr std::string_view UDP_PORT_MEMBER = "udpPort";
constexpr std::string_view GAME_VERSION_MEMBER = "gameVersion";
constexpr std::string_view PROTOCOL_VERSION_MEMBER = "protocolVersion";
constexpr std::string_view MAXIMUM_GAMES_MEMBER = "maximumGames";
constexpr std::string_view MAXIMUM_PLAYERS_PER_GAME_MEMBER =
    "maximumPlayersPerGame";

/** @brief The members of a heartbeat and of each game it lists, in JSON. */
constexpr std::string_view GAMES_MEMBER = "games";
constexpr std::string_view MEASURES_MEMBER = "measures";
constexpr std::string_view NUMBER_MEMBER = "number";
constexpr std::string_view STATE_MEMBER = "state";
constexpr std::string_view DIFFICULTY_MEMBER = "difficulty";
constexpr std::string_view LIVES_MEMBER = "lives";
constexpr std::string_view FRIENDLY_FIRE_MEMBER = "friendlyFire";
constexpr std::string_view DIFFICULTIES_MEMBER = "difficulties";

/** @brief The fewest difficulties a heartbeat names, and a list of games: a
 * server always holds one, and a server that sent no heartbeat lists none. */
constexpr std::size_t FEWEST_HEARTBEAT_DIFFICULTIES = 1;
constexpr std::size_t FEWEST_LISTED_DIFFICULTIES = 0;
constexpr std::string_view PLAYERS_MEMBER = "players";

/** @brief The members of the measures of a heartbeat, in JSON. */
constexpr std::string_view TICK_P50_MEMBER = "tickP50Milliseconds";
constexpr std::string_view TICK_P95_MEMBER = "tickP95Milliseconds";
constexpr std::string_view RECEIVED_BYTES_MEMBER = "receivedBytesPerSecond";
constexpr std::string_view SENT_BYTES_MEMBER = "sentBytesPerSecond";
constexpr std::string_view LOSS_PERCENT_MEMBER = "lossPercent";
constexpr std::string_view AVERAGE_PING_MEMBER = "averagePingMilliseconds";
constexpr std::string_view CPU_PERCENT_MEMBER = "cpuPercent";
constexpr std::string_view USED_MEMORY_MEMBER = "usedMemoryBytes";
constexpr std::string_view TOTAL_MEMORY_MEMBER = "totalMemoryBytes";

/** @brief The member of the answer to a heartbeat that lists the orders. */
constexpr std::string_view COMMANDS_MEMBER = "commands";

/** @brief The members of a created server, beside NAME_MEMBER. */
constexpr std::string_view ID_MEMBER = "id";
constexpr std::string_view KEY_MEMBER = "key";

/** @brief The member of a registration that holds its probe secret, and of a
 * heartbeat that holds the token of a probe. */
constexpr std::string_view PROBE_SECRET_MEMBER = "probeSecret";
constexpr std::string_view PROBE_TOKEN_MEMBER = "probeToken";

/**
 * @brief The bounds of a probe secret, which a column INTEGER of SQLite holds
 * as it is, and of a token, which CookieIssuer never makes 0.
 */
constexpr std::uint64_t LOWEST_PROBE_SECRET = 1;
constexpr std::uint64_t HIGHEST_PROBE_SECRET =
    std::numeric_limits<std::int64_t>::max();
constexpr std::uint64_t LOWEST_PROBE_TOKEN = 1;
constexpr std::uint64_t HIGHEST_PROBE_TOKEN =
    std::numeric_limits<std::uint64_t>::max();

/** @brief The lowest and the highest id of a game server. */
constexpr std::uint64_t FIRST_SERVER_ID = 1;
constexpr std::uint64_t HIGHEST_SERVER_ID =
    std::numeric_limits<std::int64_t>::max();

/**
 * @brief The key of a game server: SERVER_KEY_PREFIX, so a key that leaks in a
 * log or a repository is recognized, then SERVER_KEY_BYTES random bytes in
 * lowercase hexadecimal, SERVER_KEY_SIZE characters in all.
 */
constexpr std::string_view SERVER_KEY_PREFIX = "rtk_";
constexpr std::size_t SERVER_KEY_BYTES = 32;
constexpr std::size_t SERVER_KEY_SIZE =
    SERVER_KEY_PREFIX.size() + (SERVER_KEY_BYTES * HEX_DIGITS_PER_BYTE);
constexpr std::string_view SERVER_KEY_DIGITS = LOWERCASE_HEX_DIGITS;

/** @brief How each HostedGameState is written, in the order of the enum. */
constexpr std::string_view WAITING_STATE_TEXT = "waiting";
constexpr std::string_view STARTING_STATE_TEXT = "starting";
constexpr std::string_view PLAYING_STATE_TEXT = "playing";
constexpr std::string_view FINISHED_STATE_TEXT = "finished";
constexpr std::string_view FULL_STATE_TEXT = "full";
constexpr auto HOSTED_GAME_STATE_TEXTS = std::to_array<std::string_view>(
    {WAITING_STATE_TEXT, STARTING_STATE_TEXT, PLAYING_STATE_TEXT,
     FINISHED_STATE_TEXT, FULL_STATE_TEXT});

/** @brief How each ListedGameState is written, in the order of the enum. */
constexpr std::string_view IN_GAME_STATE_TEXT = "inGame";
constexpr auto LISTED_GAME_STATE_TEXTS = std::to_array<std::string_view>(
    {WAITING_STATE_TEXT, IN_GAME_STATE_TEXT, FULL_STATE_TEXT});

/** @brief How each ServerStatus is written, in the order of the enum. */
constexpr std::string_view ONLINE_STATUS_TEXT = "online";
constexpr std::string_view DEGRADED_STATUS_TEXT = "degraded";
constexpr std::string_view OFFLINE_STATUS_TEXT = "offline";
constexpr std::string_view MAINTENANCE_STATUS_TEXT = "maintenance";
constexpr std::string_view UNREACHABLE_STATUS_TEXT = "unreachable";
constexpr auto SERVER_STATUS_TEXTS = std::to_array<std::string_view>(
    {ONLINE_STATUS_TEXT, DEGRADED_STATUS_TEXT, OFFLINE_STATUS_TEXT,
     MAINTENANCE_STATUS_TEXT, UNREACHABLE_STATUS_TEXT});

/**
 * @brief The members of a listed server beside those of a registration, of a
 * sample, and of a dated status; a date is written in seconds since 1970
 * UTC.
 */
constexpr std::string_view STATUS_MEMBER = "status";
constexpr std::string_view GAME_COUNT_MEMBER = "gameCount";
constexpr std::string_view TAKEN_AT_MEMBER = "takenAt";
constexpr std::string_view SINCE_MEMBER = "since";

/** @brief The member that holds the items of each list. */
constexpr std::string_view SERVERS_MEMBER = "servers";
constexpr std::string_view SAMPLES_MEMBER = "samples";
constexpr std::string_view STATUSES_MEMBER = "statuses";

/** @brief The member of a listed game that counts its players. */
constexpr std::string_view PLAYER_COUNT_MEMBER = "playerCount";

/** @brief The fewest and the most lives a hosted or listed game may give its
 * players: what one byte holds, above 0. A server checks its own, narrower
 * bounds when a game is created. */
constexpr std::uint64_t LOWEST_GAME_LIVES = 1;
constexpr std::uint64_t HIGHEST_GAME_LIVES =
    std::numeric_limits<std::uint8_t>::max();

/** @brief The fewest players a listed game may hold. */
constexpr std::uint64_t MINIMUM_LISTED_PLAYERS = 0;

/** @brief The members of a finished game and of each of its players. */
constexpr std::string_view PLAYED_AT_MEMBER = "playedAt";
constexpr std::string_view VANILLA_MEMBER = "vanilla";
constexpr std::string_view NICKNAME_MEMBER = "nickname";
constexpr std::string_view ACCOUNT_ID_MEMBER = "accountId";
constexpr std::string_view SCORE_MEMBER = "score";
constexpr std::string_view VICTORY_MEMBER = "victory";
constexpr std::string_view WON_LOOKS_MEMBER = "wonLooks";

/** @brief The most look items a row of a result may name: a game server
 * reports each item a player won once, and the catalog holds
 * identifiers::SHIP_LOOK_ITEM_COUNT of them. */
constexpr std::size_t MAXIMUM_WON_LOOKS = identifiers::SHIP_LOOK_ITEM_COUNT;

/** @brief The bounds of the rank of a row of a leaderboard: 1 for the best
 * score, at most one rank per player kept, whom a column INTEGER of SQLite
 * counts. */
constexpr std::uint64_t LOWEST_RANK = 1;
constexpr std::uint64_t HIGHEST_RANK = std::numeric_limits<std::int64_t>::max();

/** @brief The members of a leaderboard and of each of its rows. */
constexpr std::string_view SCORES_MEMBER = "scores";
constexpr std::string_view PLAYER_MEMBER = "player";
constexpr std::string_view RANK_MEMBER = "rank";
constexpr std::string_view SERVER_ID_MEMBER = "serverId";
constexpr std::string_view SERVER_NAME_MEMBER = "serverName";

/**
 * @brief The id of a game a server draws, `lower(hex(randomblob(16)))` in
 * SQLite: GAME_ID_BYTES random bytes in lowercase hexadecimal, GAME_ID_SIZE
 * characters.
 */
constexpr std::size_t GAME_ID_BYTES = 16;
constexpr std::size_t GAME_ID_SIZE = GAME_ID_BYTES * HEX_DIGITS_PER_BYTE;
constexpr std::string_view GAME_ID_DIGITS = LOWERCASE_HEX_DIGITS;

/** @brief The longest id of an account, in bytes of printable UTF-8. */
constexpr std::size_t MAXIMUM_ACCOUNT_ID_SIZE = 64;

/** @brief The members of credentials, of an account and of a session. */
constexpr std::string_view PASSWORD_MEMBER = "password";
constexpr std::string_view ROLE_MEMBER = "role";
constexpr std::string_view TOKEN_MEMBER = "token";
constexpr std::string_view EXPIRES_AT_MEMBER = "expiresAt";
constexpr std::string_view USER_MEMBER = "account";

/** @brief The members of a game ticket: the ticket itself, and whether a
 * result already paid through it. */
constexpr std::string_view TICKET_MEMBER = "ticket";
constexpr std::string_view USED_MEMBER = "used";

/**
 * @brief The bounds of a password, in bytes of printable UTF-8: the bounds
 * Better Auth sets by default, 8 to 128. A password is hashed whatever its
 * length, so the upper bound only caps the work of one request.
 */
constexpr std::size_t MINIMUM_PASSWORD_SIZE = 8;
constexpr std::size_t MAXIMUM_PASSWORD_SIZE = 128;

/** @brief How each UserRole is written, in the order of the enum. */
constexpr std::string_view PLAYER_ROLE_TEXT = "player";
constexpr std::string_view ADMIN_ROLE_TEXT = "admin";
constexpr auto USER_ROLE_TEXTS =
    std::to_array<std::string_view>({PLAYER_ROLE_TEXT, ADMIN_ROLE_TEXT});

/**
 * @brief The token of a session: SESSION_TOKEN_PREFIX, so a token that leaks
 * is recognized, then SESSION_TOKEN_BYTES random bytes in lowercase
 * hexadecimal, SESSION_TOKEN_SIZE characters in all.
 */
constexpr std::string_view SESSION_TOKEN_PREFIX = "rts_";
constexpr std::size_t SESSION_TOKEN_BYTES = 32;
constexpr std::size_t SESSION_TOKEN_SIZE =
    SESSION_TOKEN_PREFIX.size() + (SESSION_TOKEN_BYTES * HEX_DIGITS_PER_BYTE);

/** @brief The bounds of a score, what a server keeps of one (32 bits). */
constexpr std::uint64_t LOWEST_SCORE = 0;
constexpr std::uint64_t HIGHEST_SCORE =
    std::numeric_limits<std::uint32_t>::max();

/** @brief The bounds of the date a game ended, in seconds since 1970 UTC: what
 * a column INTEGER of SQLite holds. */
constexpr std::uint64_t EARLIEST_PLAYED_AT = 0;
constexpr std::uint64_t LATEST_PLAYED_AT =
    std::numeric_limits<std::int64_t>::max();

/** @brief The fewest players a finished game names. */
constexpr std::size_t FEWEST_FINISHED_GAME_PLAYERS = 1;

/** @brief The longest name of a game, in bytes of UTF-8. */
constexpr std::size_t MAXIMUM_GAME_NAME_SIZE = 16;

/**
 * @brief The fewest and the most games a server may host; the first is
 * numbered FIRST_HOSTED_GAME_NUMBER (ProtocolConstants.hpp). A supervisor runs
 * 8 at most today (MAXIMUM_RUNNING_GAMES of the server); a heartbeat of 64
 * games of four players, every nickname and difficulty of 16 bytes and every
 * measure taken, is 9.8 KiB of JSON.
 */
constexpr std::uint16_t MINIMUM_HOSTED_GAMES = 1;
constexpr std::uint16_t MAXIMUM_HOSTED_GAMES = 64;

/** @brief The fewest players a game of a server may take. */
constexpr std::uint8_t MINIMUM_PLAYERS_PER_GAME = 1;

/** @brief The lowest and the highest version of the UDP protocol. */
constexpr std::uint8_t LOWEST_PROTOCOL_VERSION = 0;
constexpr std::uint8_t HIGHEST_PROTOCOL_VERSION =
    std::numeric_limits<std::uint8_t>::max();

/** @brief The highest value of a measure, of a share of lost datagrams and
 * of a share of busy processors. */
constexpr double MAXIMUM_MEASURE = std::numeric_limits<double>::max();
constexpr double MAXIMUM_LOSS_PERCENT = 100.0;
constexpr double MAXIMUM_CPU_PERCENT = 100.0;

/**
 * @brief How often a game server tells the master its state when nothing
 * changes; the master counts the heartbeats a server missed in this interval.
 */
constexpr std::chrono::milliseconds MASTER_HEARTBEAT_INTERVAL{5'000};

/**
 * @brief The least time between two heartbeats of a game server when its games
 * change; the master probes one address and port at most this often.
 */
constexpr std::chrono::milliseconds MINIMUM_NOTICE_GAP{1'000};

/** @brief The members of a wallet, of a line of its journal, of an exchange
 * and of the looks an account owns. */
constexpr std::string_view KOPECKS_MEMBER = "kopecks";
constexpr std::string_view CHIPS_MEMBER = "chips";
constexpr std::string_view MOVEMENTS_MEMBER = "movements";
constexpr std::string_view CURRENCY_MEMBER = "currency";
constexpr std::string_view AMOUNT_MEMBER = "amount";
constexpr std::string_view REASON_MEMBER = "reason";
constexpr std::string_view MADE_AT_MEMBER = "madeAt";
constexpr std::string_view FROM_MEMBER = "from";
constexpr std::string_view WALLET_MEMBER = "wallet";
constexpr std::string_view ITEMS_MEMBER = "items";
constexpr std::string_view ITEM_MEMBER = "item";

/** @brief How each Currency is written, in the order of the enum. */
constexpr std::string_view KOPECKS_CURRENCY_TEXT = "kopecks";
constexpr std::string_view CHIPS_CURRENCY_TEXT = "chips";
constexpr auto CURRENCY_TEXTS = std::to_array<std::string_view>(
    {KOPECKS_CURRENCY_TEXT, CHIPS_CURRENCY_TEXT});

/** @brief How each MovementReason is written, in the order of the enum. */
constexpr auto MOVEMENT_REASON_TEXTS = std::to_array<std::string_view>(
    {"opening", "game", "exchange", "purchase", "stake", "refund", "payout"});

/** @brief The largest amount a request of the cashier may name: any amount
 * the wire carries; the master refuses one of none, or one past the largest
 * balance, as an amount it cannot change. */
constexpr std::uint64_t MAXIMUM_EXCHANGE_AMOUNT =
    std::numeric_limits<std::uint32_t>::max();

/** @brief The members of the rooms of the casino, of their state, and of
 * the bets of a round. */
constexpr std::string_view ROOMS_MEMBER = "rooms";
constexpr std::string_view SEATS_MEMBER = "seats";
constexpr std::string_view SEATED_MEMBER = "seated";
constexpr std::string_view ROUND_MEMBER = "round";
constexpr std::string_view PHASE_MEMBER = "phase";
constexpr std::string_view CLOSES_AT_MEMBER = "closesAt";
constexpr std::string_view RESULT_AT_MEMBER = "resultAt";
constexpr std::string_view ENDS_AT_MEMBER = "endsAt";
constexpr std::string_view SERVER_TIME_MEMBER = "serverTime";
constexpr std::string_view WINNING_NUMBER_MEMBER = "winningNumber";
constexpr std::string_view HISTORY_MEMBER = "history";
constexpr std::string_view STAKE_MEMBER = "stake";
constexpr std::string_view PAYOUT_MEMBER = "payout";
constexpr std::string_view BETS_MEMBER = "bets";
constexpr std::string_view KIND_MEMBER = "kind";
constexpr std::string_view FIRST_NUMBER_MEMBER = "firstNumber";

/** @brief The longest name of a room of the casino, in bytes of UTF-8. */
constexpr std::size_t MAXIMUM_CASINO_ROOM_NAME_SIZE = 32;

/** @brief How each RouletteBetKind is written, in the order of the enum. */
constexpr auto ROULETTE_BET_KIND_TEXTS = std::to_array<std::string_view>(
    {"straight", "split", "splitAcross", "street", "corner", "sixLine", "dozen",
     "column", "red", "black", "even", "odd", "low", "high"});

/** @brief How each RoulettePhase is written, in the order of the enum. */
constexpr auto ROULETTE_PHASE_TEXTS =
    std::to_array<std::string_view>({"betting", "spinning", "result"});

/**
 * @brief The most bets a body of bets may list, and the most chips one bet of
 * it may hold, as the wire carries them. The master reads a list up to twice
 * MAXIMUM_ROULETTE_BETS, so it answers a list just over the limit, or a bet
 * over the cap of a round, with the rule it breaks; a longer list is refused
 * as a malformed body.
 */
constexpr std::size_t MAXIMUM_READ_ROULETTE_BETS = 64;
constexpr std::uint64_t MAXIMUM_READ_BET_AMOUNT =
    std::numeric_limits<std::uint32_t>::max();

/** @brief The largest round a message may name, and the largest date in
 * milliseconds: any whole number of 64 signed bits. */
constexpr std::uint64_t HIGHEST_ROUND =
    std::numeric_limits<std::int64_t>::max();
constexpr std::uint64_t LATEST_MILLISECONDS =
    std::numeric_limits<std::int64_t>::max();

}  // namespace rtype::network
