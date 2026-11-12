#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

/** @brief The routes of the results and of the leaderboard, as a client calls
 * them. */
constexpr std::string_view OWN_RESULT_URL_PREFIX = "/api/servers/me/results/";
constexpr std::string_view LEADERBOARD_URL = "/api/leaderboard";

/** @brief The second server the tests create, beside TEST_SERVER_NAME. */
constexpr std::string_view LYON_SERVER_NAME = "Lyon 1";

/** @brief Ids of games as a server draws them, and an id it never draws. */
constexpr std::string_view FIRST_GAME_ID = "00000000000000000000000000000001";
constexpr std::string_view SECOND_GAME_ID = "00000000000000000000000000000002";
constexpr std::string_view THIRD_GAME_ID = "00000000000000000000000000000003";
constexpr std::string_view MALFORMED_GAME_ID = "game-1";

/** @brief A difficulty beside PARIS_DIFFICULTY, and numbers of players. */
constexpr std::string_view HARD_DIFFICULTY = "hard";
constexpr std::uint8_t ONE_PLAYER = 1;
constexpr std::uint8_t TWO_PLAYERS = 2;

/** @brief Another nickname of the second pilot of the network tests, who
 * plays under SECOND_PILOT_ACCOUNT, beside SECOND_PILOT. */
constexpr std::string_view OTHER_NICKNAME_OF_SECOND_PILOT = "HUNTER_2";

/** @brief The account of the second pilot in capital letters: another
 * account, since an id is compared as written. */
constexpr std::string_view SECOND_PILOT_ACCOUNT_IN_CAPITALS = "ACCOUNT-42";

/** @brief A score between FIRST_PILOT_SCORE and SECOND_PILOT_SCORE. */
constexpr std::uint32_t MIDDLE_SCORE = 5'000;

/** @brief When the games ended. */
constexpr std::chrono::sys_seconds GAME_DATE{
    std::chrono::seconds{1'800'000'000}};

/** @brief The rank of the third score of a leaderboard whose first two are
 * equal. */
constexpr std::int64_t THIRD_RANK = 3;

/** @brief Queries of the leaderboard, after its path. */
constexpr std::string_view NORMAL_FOR_TWO_QUERY =
    "?difficulty=normal&playerCount=2";
constexpr std::string_view HARD_FOR_TWO_QUERY =
    "?difficulty=hard&playerCount=2";
constexpr std::string_view NORMAL_FOR_ONE_QUERY =
    "?difficulty=normal&playerCount=1";
constexpr std::string_view SERVER_QUERY_START = "&server=";
constexpr std::string_view ASKING_BYDO_HUNTER_QUERY =
    "?difficulty=normal&playerCount=2&player=BYDO_HUNTER";
constexpr std::string_view FIRST_ROW_AND_R9_QUERY =
    "?difficulty=normal&playerCount=2&limit=1&player=r9";

/** @brief Queries that name no difficulty, then no number of players, then
 * neither: each reads every value of what it leaves out. */
constexpr std::string_view EVERY_DIFFICULTY_FOR_TWO_QUERY = "?playerCount=2";
constexpr std::string_view NORMAL_FOR_EVERY_COUNT_QUERY = "?difficulty=normal";
constexpr std::string_view EVERY_GAME_QUERY;

/** @brief Queries the master refuses: an empty difficulty, a number of players
 * or a server out of its bounds, a nickname too long. */
constexpr std::string_view EMPTY_DIFFICULTY_QUERY =
    "?difficulty=&playerCount=2";
constexpr std::string_view FIVE_PLAYERS_QUERY =
    "?difficulty=normal&playerCount=5";
constexpr std::string_view SERVER_OF_ZERO_QUERY =
    "?difficulty=normal&playerCount=2&server=0";
constexpr std::string_view PLAYER_TOO_LONG_QUERY =
    "?difficulty=normal&playerCount=2&player=ABCDEFGHIJKLMNOPQ";

/** @brief The members of an answer of the leaderboard the tests read. */
constexpr std::string_view SCORES_MEMBER_NAME = "scores";
constexpr std::string_view PLAYER_MEMBER_NAME = "player";
constexpr std::string_view NICKNAME_MEMBER_NAME = "nickname";
constexpr std::string_view RANK_MEMBER_NAME = "rank";
constexpr std::string_view SCORE_MEMBER_NAME = "score";
constexpr std::string_view SERVER_NAME_MEMBER_NAME = "serverName";

/** @brief What the refusal of a game that was not vanilla says. */
constexpr std::string_view NOT_VANILLA_DETAIL_PART = "not vanilla";
