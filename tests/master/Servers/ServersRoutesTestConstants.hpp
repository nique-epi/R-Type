#pragma once

#include <string_view>

/** @brief The routes of the game servers, as a client calls them. */
constexpr std::string_view SERVERS_URL = "/api/servers";
constexpr std::string_view OWN_SERVER_URL = "/api/servers/me";
constexpr std::string_view OWN_HEARTBEAT_URL = "/api/servers/me/heartbeat";
constexpr std::string_view POST_METHOD = "POST";
constexpr std::string_view GET_METHOD_TEXT = "GET";

/** @brief Where the games of a server are listed, around its id. */
constexpr std::string_view SERVER_GAMES_PATH_PREFIX = "/api/servers/";
constexpr std::string_view SERVER_GAMES_PATH_SUFFIX = "/games";

/** @brief Ids that name no server: unused, not a number, zero, negative. */
constexpr std::string_view UNUSED_SERVER_ID = "999";
constexpr std::string_view NOT_A_SERVER_ID = "abc";
constexpr std::string_view ZERO_SERVER_ID = "0";
constexpr std::string_view NEGATIVE_SERVER_ID = "-1";

/** @brief The games of "Paris 1" as the route lists them. */
constexpr std::string_view PARIS_GAMES_BODY =
    R"({"games":[{"name":"GAME 1","number":1,"playerCount":1,"state":"waiting"},)"
    R"({"name":"GAME 2","number":2,"playerCount":2,"state":"inGame"}]})";
constexpr std::string_view NO_GAMES_BODY = R"({"games":[]})";

/** @brief The parts of the history of a server, after its id. */
constexpr std::string_view SAMPLES_PATH_SUFFIX = "/samples";
constexpr std::string_view STATUSES_PATH_SUFFIX = "/statuses";

/** @brief The queries of a list: none, and four out of bounds. */
constexpr std::string_view NO_QUERY;
constexpr std::string_view LIMIT_ZERO_QUERY = "?limit=0";
constexpr std::string_view LIMIT_OVER_HUNDRED_QUERY = "?limit=101";
constexpr std::string_view LIMIT_NOT_A_NUMBER_QUERY = "?limit=abc";
constexpr std::string_view NEGATIVE_OFFSET_QUERY = "?offset=-1";

/** @brief What a page out of bounds is refused with. */
constexpr std::string_view EXPECTED_INVALID_PAGE_DETAIL =
    "limit is a whole number from 1 to 100, and offset one from 0";

/** @brief "Paris 1" in the list, but for its id and its status, which the
 * tests read apart. */
constexpr std::string_view PARIS_LISTED_SERVER_BODY =
    R"({"address":"203.0.113.7","gameCount":2,"gameVersion":"0.4.0",)"
    R"("maximumGames":8,"maximumPlayersPerGame":4,"name":"Paris 1",)"
    R"("playerCount":3,"protocolVersion":2,"udpPort":4242})";
constexpr std::string_view NO_SERVERS_BODY = R"({"servers":[]})";

/** @brief The measures of the heartbeat of "Paris 1", as a sample keeps them.
 */
constexpr std::string_view PARIS_MEASURES_BODY =
    R"({"lossPercent":0.5,"receivedBytesPerSecond":12000.0,)"
    R"("sentBytesPerSecond":34000.0,"tickP50Milliseconds":1.5,)"
    R"("tickP95Milliseconds":4.25})";

/** @brief The members of the lists the tests read. */
constexpr std::string_view SERVERS_MEMBER_NAME = "servers";
constexpr std::string_view SAMPLES_MEMBER_NAME = "samples";
constexpr std::string_view STATUSES_MEMBER_NAME = "statuses";
constexpr std::string_view STATUS_MEMBER_NAME = "status";
constexpr std::string_view TAKEN_AT_MEMBER_NAME = "takenAt";
constexpr std::string_view MEASURES_MEMBER_NAME = "measures";
constexpr std::string_view GAME_COUNT_MEMBER_NAME = "gameCount";
constexpr std::string_view PLAYER_COUNT_MEMBER_NAME = "playerCount";

/** @brief The games and players of the heartbeat of "Paris 1". */
constexpr int PARIS_GAME_COUNT = 2;
constexpr int PARIS_PLAYER_COUNT = 3;
constexpr std::string_view GAMES_MEMBER_NAME = "games";
constexpr std::string_view STATE_MEMBER_NAME = "state";
constexpr std::string_view FULL_STATE_NAME = "full";
constexpr std::string_view IN_GAME_STATE_NAME = "inGame";

/** @brief How a key is sent, and the challenge of an answer without one. */
constexpr std::string_view BEARER_PREFIX_TEXT = "Bearer ";
constexpr std::string_view CHALLENGE_HEADER_NAME = "WWW-Authenticate";
constexpr std::string_view EXPECTED_CHALLENGE = "Bearer";

/** @brief Where a created server lives, followed by its id. */
constexpr std::string_view EXPECTED_SERVER_LOCATION_PREFIX = "/api/servers/";

/** @brief The body of a creation, and one that names no server. */
constexpr std::string_view PARIS_CREATION_BODY = R"({"name":"Paris 1"})";
constexpr std::string_view EMPTY_OBJECT_BODY = "{}";
constexpr std::string_view NAME_POINTER_TEXT = "/name";

/** @brief The members of the answers the tests read. */
constexpr std::string_view ID_MEMBER_NAME = "id";
constexpr std::string_view KEY_MEMBER_NAME = "key";
constexpr std::string_view ADDRESS_MEMBER_NAME = "address";
constexpr std::string_view COMMANDS_MEMBER_NAME = "commands";
