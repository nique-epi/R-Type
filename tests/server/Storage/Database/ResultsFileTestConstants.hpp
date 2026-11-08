#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The game vanillaGameOf() builds: its difficulty, its one pilot and
 * the score of that pilot. */
constexpr std::string_view SAVED_GAME_DIFFICULTY = "normal";
constexpr std::uint8_t ONE_PILOT = 1;
constexpr std::uint32_t SAVED_PILOT_SCORE = 12'000;

/** @brief How a test counts the rows of a game, those sent and those
 * refused. */
constexpr std::string_view ANSWERS_OF_A_GAME_QUERY =
    "SELECT COUNT(*), COUNT(sent_at), COUNT(refused_at) FROM game_results "
    "WHERE game_id = :game_id";
constexpr std::string_view SAVED_GAME_ID_PARAMETER = ":game_id";
constexpr int ROWS_COLUMN = 0;
constexpr int SENT_ROWS_COLUMN = 1;
constexpr int REFUSED_ROWS_COLUMN = 2;

}  // namespace rtype::server::testing
