#pragma once

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

/** @brief A server of no game, and games of five players: neither exists. */
constexpr std::uint64_t NO_GAME = 0;
constexpr std::uint64_t FIVE_PLAYERS = 5;

/** @brief The two games of the heartbeat of "Paris 1". */
constexpr std::uint16_t WAITING_GAME_NUMBER = 1;
constexpr std::uint16_t PLAYING_GAME_NUMBER = 2;
constexpr std::string_view PARIS_DIFFICULTY = "normal";
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
constexpr std::string_view MAXIMUM_GAMES_POINTER = "/maximumGames";
constexpr std::string_view MAXIMUM_PLAYERS_PER_GAME_POINTER =
    "/maximumPlayersPerGame";
constexpr std::string_view SECOND_GAME_NUMBER_POINTER = "/games/1/number";
constexpr std::string_view FIRST_GAME_STATE_POINTER = "/games/0/state";
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

/** @brief How a refusal quotes the pointer it names, and the whole message. */
constexpr std::string_view POINTER_QUOTE = "\"";
constexpr std::string_view QUOTED_WHOLE_MESSAGE = "\"\"";

}  // namespace rtype::network::testing
