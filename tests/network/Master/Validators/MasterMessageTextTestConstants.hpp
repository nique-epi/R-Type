#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::network::testing {

/** @brief The registration of "Paris 1", which the tests send and read. */
constexpr std::string_view PARIS_NAME = "Paris 1";
constexpr std::string_view PARIS_ADDRESS = "203.0.113.7";
constexpr std::uint16_t PARIS_UDP_PORT = 4242;
constexpr std::string_view PARIS_GAME_VERSION = "0.4.0";
constexpr std::uint8_t PARIS_PROTOCOL_VERSION = 2;
constexpr std::uint16_t PARIS_MAXIMUM_GAMES = 8;
constexpr std::uint8_t PARIS_MAXIMUM_PLAYERS_PER_GAME = 4;
constexpr std::uint64_t PARIS_PROBE_SECRET = 0x5A5A'5A5A'5A5A'5A5A;

/** @brief Names of 32 bytes, the longest a server may have, and of 33. */
constexpr std::string_view NAME_OF_32_BYTES =
    "abcdefghijklmnopqrstuvwxyz012345";
constexpr std::string_view NAME_OF_33_BYTES =
    "abcdefghijklmnopqrstuvwxyz0123456";

/** @brief A name holding a line feed, which a log line would split. */
constexpr std::string_view NAME_WITH_LINE_FEED = "Paris\n1";

/** @brief A host name, where the registration takes only an IPv4 address. */
constexpr std::string_view HOST_NAME_ADDRESS = "paris.example.org";

/** @brief The port written as text, where JSON must hold a number. */
constexpr std::string_view PORT_AS_TEXT = "4242";

/** @brief Ports just outside the range of UDP ports: 0 and 65536. */
constexpr std::uint64_t PORT_BELOW_THE_RANGE = 0;
constexpr std::uint64_t PORT_ABOVE_THE_RANGE = 65536;

/**
 * @brief Probe secrets just outside what the master keeps in a column INTEGER
 * of SQLite, 0 and 2^63, and the two ends it keeps, 1 and 2^63 - 1.
 */
constexpr std::uint64_t PROBE_SECRET_BELOW_ITS_RANGE = 0;
constexpr std::uint64_t PROBE_SECRET_ABOVE_ITS_RANGE =
    9'223'372'036'854'775'808U;
constexpr std::uint64_t LOWEST_KEPT_PROBE_SECRET = 1;
constexpr std::uint64_t HIGHEST_KEPT_PROBE_SECRET = 9'223'372'036'854'775'807;

/**
 * @brief A token of a probe of 0, which the master never issues, and the
 * largest token a heartbeat can carry, 2^64 - 1.
 */
constexpr std::uint64_t PROBE_TOKEN_OF_ZERO = 0;
constexpr std::uint64_t LARGEST_PROBE_TOKEN = 18'446'744'073'709'551'615U;

/** @brief A server of no game, and games of five players: neither exists. */
constexpr std::uint64_t NO_GAME = 0;
constexpr std::uint64_t FIVE_PLAYERS = 5;

/** @brief The two games of the heartbeat of "Paris 1". */
constexpr std::uint16_t WAITING_GAME_NUMBER = 1;
constexpr std::uint16_t PLAYING_GAME_NUMBER = 2;
constexpr std::string_view GAME_NAME_PREFIX = "GAME ";
constexpr std::string_view WAITING_GAME_NAME = "GAME 1";
constexpr std::string_view PLAYING_GAME_NAME = "GAME 2";
constexpr std::string_view PARIS_DIFFICULTY = "normal";
constexpr std::uint8_t WAITING_GAME_PLAYER_COUNT = 1;
constexpr std::uint8_t PLAYING_GAME_PLAYER_COUNT = 2;
constexpr std::string_view FIRST_PILOT = "ACE";
constexpr std::string_view SECOND_PILOT = "BYDO_HUNTER";
constexpr std::string_view THIRD_PILOT = "R9";

/** @brief The measures the heartbeat of "Paris 1" carries; no ping. */
constexpr double PARIS_TICK_P50 = 1.5;
constexpr double PARIS_TICK_P95 = 4.25;
constexpr double PARIS_RECEIVED_BYTES = 12000.0;
constexpr double PARIS_SENT_BYTES = 34000.0;
constexpr double PARIS_LOSS_PERCENT = 0.5;

/** @brief Values just outside what a game or a measure may hold. */
constexpr std::string_view PAUSED_STATE = "paused";
constexpr std::string_view FULL_STATE = "full";
constexpr std::string_view PLAYING_STATE = "playing";
constexpr std::string_view GAME_NAME_OF_17_BYTES = "ABCDEFGHIJKLMNOPQ";
constexpr std::string_view EMPTY_GAME_NAME{};
constexpr double NEGATIVE_MEASURE = -1.0;
constexpr double LOSS_ABOVE_ALL = 100.5;
constexpr std::size_t GAMES_ABOVE_THE_MOST = 65;
constexpr std::string_view FOURTH_PILOT = "SPARE";
constexpr std::string_view FIFTH_PILOT = "ONE_TOO_MANY";

/** @brief A text that is no JSON, and a member no message knows. */
constexpr std::string_view NOT_JSON = "{\"name\":";
constexpr std::string_view UNKNOWN_MEMBER_POINTER = "/region";
constexpr std::string_view UNKNOWN_MEMBER_VALUE = "eu-west";

/**
 * @brief The JSON Pointers (RFC 6901) of the members the tests break, as the
 * refusal must name them.
 */
constexpr std::string_view NAME_POINTER = "/name";
constexpr std::string_view ADDRESS_POINTER = "/address";
constexpr std::string_view UDP_PORT_POINTER = "/udpPort";
constexpr std::string_view PROBE_SECRET_POINTER = "/probeSecret";
constexpr std::string_view PROBE_TOKEN_POINTER = "/probeToken";
constexpr std::string_view MAXIMUM_GAMES_POINTER = "/maximumGames";
constexpr std::string_view MAXIMUM_PLAYERS_PER_GAME_POINTER =
    "/maximumPlayersPerGame";
constexpr std::string_view SECOND_GAME_NUMBER_POINTER = "/games/1/number";
constexpr std::string_view FIRST_GAME_STATE_POINTER = "/games/0/state";
constexpr std::string_view FIRST_GAME_NAME_POINTER = "/games/0/name";
constexpr std::string_view FIRST_GAME_PLAYER_COUNT_POINTER =
    "/games/0/playerCount";

/** @brief The listing of the games of "Paris 1", as the master writes it. */
constexpr std::string_view PARIS_GAME_LISTING_TEXT =
    R"({"games":[{"name":"GAME 1","number":1,"playerCount":1,"state":"waiting"},)"
    R"({"name":"GAME 2","number":2,"playerCount":2,"state":"inGame"}]})";
constexpr std::string_view FIRST_GAME_FIRST_PLAYER_POINTER =
    "/games/0/players/0";
constexpr std::string_view FIRST_GAME_PLAYERS_POINTER = "/games/0/players";
constexpr std::string_view GAMES_POINTER = "/games";
constexpr std::string_view TICK_P50_POINTER = "/measures/tickP50Milliseconds";
constexpr std::string_view LOSS_PERCENT_POINTER = "/measures/lossPercent";
constexpr std::string_view AVERAGE_PING_POINTER =
    "/measures/averagePingMilliseconds";

/** @brief The id and the key the master gives "Paris 1" when it creates it. */
constexpr std::int64_t PARIS_ID = 7;
constexpr std::string_view PARIS_KEY =
    "rtk_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief Texts shaped almost like a key: no prefix, uppercase, one short. */
constexpr std::string_view KEY_WITHOUT_PREFIX =
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view UPPERCASE_KEY =
    "rtk_0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
constexpr std::string_view SHORT_KEY =
    "rtk_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde";
constexpr std::string_view KEY_POINTER = "/key";

/** @brief A problem document of the master, its detail, and text that is no
 * JSON. */
constexpr std::string_view CONFLICT_PROBLEM =
    R"({"status":409,"title":"Conflict","detail":"register first"})";
constexpr std::string_view CONFLICT_DETAIL = "register first";
constexpr std::string_view PLAIN_TEXT_ERROR = "Bad Gateway";

/** @brief How a refusal quotes the pointer it names, and the whole message. */
constexpr std::string_view POINTER_QUOTE = "\"";
constexpr std::string_view QUOTED_WHOLE_MESSAGE = "\"\"";

/**
 * @brief How the statuses of a server are written, and a text no status
 * has.
 */
constexpr std::string_view ONLINE_TEXT = "online";
constexpr std::string_view DEGRADED_TEXT = "degraded";
constexpr std::string_view OFFLINE_TEXT = "offline";
constexpr std::string_view MAINTENANCE_TEXT = "maintenance";
constexpr std::string_view UNREACHABLE_TEXT = "unreachable";
constexpr std::string_view UNKNOWN_STATUS_TEXT = "lost";

/**
 * @brief The games and players of the heartbeat of "Paris 1", as a list of
 * servers and a sample count them, and the date of a sample.
 */
constexpr std::uint16_t LISTED_GAME_COUNT = 2;
constexpr std::uint16_t LISTED_PLAYER_COUNT = 3;
constexpr std::chrono::sys_seconds SAMPLE_DATE{
    std::chrono::seconds{1'800'000'000}};

/** @brief "Paris 1" online in a list of servers, as the master writes it. */
constexpr std::string_view PARIS_SERVER_LISTING_TEXT =
    R"({"servers":[{"address":"203.0.113.7","gameCount":2,)"
    R"("gameVersion":"0.4.0","id":7,"maximumGames":8,)"
    R"("maximumPlayersPerGame":4,"name":"Paris 1","playerCount":3,)"
    R"("protocolVersion":2,"status":"online","udpPort":4242}]})";

/** @brief A sample with its tick p95 and its loss only, as the master writes
 * it. */
constexpr std::string_view PARIS_SAMPLE_LISTING_TEXT =
    R"({"samples":[{"gameCount":2,)"
    R"("measures":{"lossPercent":0.5,"tickP95Milliseconds":4.25},)"
    R"("playerCount":3,"takenAt":1800000000}]})";

/** @brief A change to degraded, as the master writes it. */
constexpr std::string_view DEGRADED_STATUS_LISTING_TEXT =
    R"({"statuses":[{"since":1800000000,"status":"degraded"}]})";

}  // namespace rtype::network::testing
