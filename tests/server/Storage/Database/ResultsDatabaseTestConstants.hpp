#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

constexpr std::string_view RESULTS_FILE_NAME = "results.db";
constexpr std::string_view NORMAL = "normal";
constexpr std::string_view HARD = "hard";
constexpr std::uint8_t TWO_PLAYERS = 2;
constexpr std::uint8_t THREE_PLAYERS = 3;
constexpr std::chrono::sys_seconds SOME_DATE{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds LATER_DATE =
    SOME_DATE + std::chrono::minutes{1};
constexpr std::chrono::sys_seconds EVEN_LATER_DATE =
    LATER_DATE + std::chrono::minutes{1};
constexpr std::string_view ACE = "ACE";
constexpr std::string_view ACE_IN_LOWERCASE = "ace";
constexpr std::string_view BOB = "BOB";
constexpr std::string_view CID = "CID";
constexpr std::string_view STRANGER = "STRANGER";
constexpr std::string_view SQL_IN_A_NICKNAME = "');DROP TABLE x";
constexpr std::uint32_t LOW_SCORE = 100;
constexpr std::uint32_t MIDDLE_SCORE = 200;
constexpr std::uint32_t HIGH_SCORE = 300;
constexpr std::uint32_t TWO_DEATHS = 2;
constexpr std::uint32_t ONE_DEATH = 1;
constexpr std::size_t GAME_ID_SIZE = 32;
constexpr std::size_t PLAYERS_ON_A_FULL_BOARD = 11;
constexpr std::uint32_t LAST_RANK_ON_A_FULL_BOARD = 11;
constexpr int DIED_AT_ONCE_EXIT_CODE = 42;

/** @brief How many games a test reads of the games the master has not
 * answered: room for every game a test saves, or for two of three. */
constexpr std::size_t EVERY_WAITING_GAME = 10;
constexpr std::size_t TWO_WAITING_GAMES = 2;

/** @brief When a test says the master answered a game. */
constexpr std::chrono::sys_seconds ANSWER_DATE =
    EVEN_LATER_DATE + std::chrono::minutes{1};

/** @brief The answers a test reads back from the rows of ACE. */
constexpr std::string_view SELECT_ANSWERS_OF_ACE =
    "SELECT sent_at, refused_at FROM game_results WHERE nickname = 'ACE'";

/** @brief A vanilla row of ACE written in a file of the first version, as a
 * server built before the second one saved it. */
constexpr std::string_view INSERT_VANILLA_ROW_OF_ACE = R"sql(
INSERT INTO game_results
  (game_id, nickname, score, deaths, played_at, difficulty, player_count,
   vanilla)
VALUES
  ('0123456789abcdef0123456789abcdef', 'ACE', 300, 2, 1800000000, 'normal', 2,
   1)
)sql";
constexpr std::string_view ID_OF_THE_FIRST_VERSION_GAME =
    "0123456789abcdef0123456789abcdef";

constexpr std::size_t REPLY_QUEUE_ROOM = 4;
constexpr std::size_t FIRST_ASKER = 0;

constexpr std::string_view SELECT_ROW_OF_ACE = R"sql(
SELECT game_id, account_id, score, deaths, played_at, difficulty, player_count,
       vanilla, sent_at
FROM game_results WHERE nickname = 'ACE'
)sql";
constexpr std::string_view COUNT_GAME_IDS =
    "SELECT COUNT(DISTINCT game_id) FROM game_results";
constexpr std::string_view SELECT_STATISTICS_OF_ACE = R"sql(
SELECT games_played, best_score, total_score, deaths FROM player_statistics
WHERE player = 'ACE' COLLATE NOCASE AND NOT has_account
)sql";
constexpr std::string_view COUNT_STATISTICS =
    "SELECT COUNT(*) FROM player_statistics";
constexpr std::string_view ACCOUNT_ID = "account-7";
constexpr std::string_view GIVE_ROWS_OF_BOB_AND_CID_AN_ACCOUNT =
    "UPDATE game_results SET account_id = 'account-7' "
    "WHERE nickname IN ('BOB', 'CID')";
constexpr std::string_view SELECT_STATISTICS_OF_THE_ACCOUNT =
    "SELECT player, games_played FROM player_statistics WHERE has_account";

}  // namespace rtype::server::testing
