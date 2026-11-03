#pragma once

#include <array>
#include <string_view>

namespace rtype::server {

/**
 * @brief The first version of the file of a server.
 *
 * A nickname compares with NOCASE, which folds ASCII letters only, as
 * PlayerRoster::hasNickname() does: ACE and ace are one player. account_id and
 * sent_at stay empty and vanilla stays 0 until accounts, the link with the
 * master and the definition of a vanilla game exist. Statistics are computed
 * from the rows, so they cannot disagree with them; COALESCE gives no collation
 * of its own, hence the explicit one.
 */
constexpr std::string_view RESULTS_TABLE_MIGRATION = R"sql(
CREATE TABLE game_results (
  game_id      TEXT    NOT NULL,
  nickname     TEXT    NOT NULL COLLATE NOCASE,
  account_id   TEXT,
  score        INTEGER NOT NULL,
  deaths       INTEGER NOT NULL,
  played_at    INTEGER NOT NULL,
  difficulty   TEXT    NOT NULL,
  player_count INTEGER NOT NULL,
  vanilla      INTEGER NOT NULL DEFAULT 0,
  sent_at      INTEGER,
  PRIMARY KEY (game_id, nickname)
) STRICT;

CREATE INDEX game_results_of_a_leaderboard
  ON game_results (difficulty, player_count);

CREATE VIEW player_statistics AS
  SELECT COALESCE(account_id, nickname) AS player,
         account_id IS NOT NULL AS has_account,
         COUNT(*) AS games_played,
         MAX(score) AS best_score,
         SUM(score) AS total_score,
         SUM(deaths) AS deaths
  FROM game_results
  GROUP BY account_id IS NOT NULL,
           COALESCE(account_id, nickname) COLLATE NOCASE;
)sql";

/** @brief The versions of the file, in order: a change adds one, never edits
 * one. */
constexpr std::array<std::string_view, 1> RESULTS_MIGRATIONS{
    RESULTS_TABLE_MIGRATION};

/** @brief Draws the identifier of a game, as the SQLite documentation advises
 * for a globally unique identifier. */
constexpr std::string_view NEW_GAME_ID_QUERY =
    "SELECT lower(hex(randomblob(16)))";

constexpr std::string_view INSERT_RESULT_STATEMENT = R"sql(
INSERT INTO game_results
  (game_id, nickname, score, deaths, played_at, difficulty, player_count)
VALUES
  (:game_id, :nickname, :score, :deaths, :played_at, :difficulty, :player_count)
)sql";

/**
 * @brief The best row of each player of one leaderboard, the best score first.
 * An account is one player under any nickname. board_rank gives equal scores
 * one rank; board_place puts the earliest game first among them.
 */
constexpr std::string_view LEADERBOARD_QUERY = R"sql(
WITH best AS (
  SELECT nickname, score, played_at,
         ROW_NUMBER() OVER (
           PARTITION BY account_id IS NOT NULL,
                        COALESCE(account_id, nickname) COLLATE NOCASE
           ORDER BY score DESC, played_at ASC) AS position
  FROM game_results
  WHERE difficulty = :difficulty AND player_count = :player_count
),
ranked AS (
  SELECT nickname, score, played_at,
         RANK() OVER (ORDER BY score DESC) AS board_rank,
         ROW_NUMBER() OVER (ORDER BY score DESC, played_at ASC, nickname ASC)
           AS board_place
  FROM best
  WHERE position = 1
)
SELECT board_rank, nickname, score, played_at,
       nickname = :asker COLLATE NOCASE AS is_asker
FROM ranked
WHERE board_place <= :top_rows OR is_asker
ORDER BY board_place
)sql";

/** @brief The parameters of the statements above. */
constexpr const char* GAME_ID_PARAMETER = ":game_id";
constexpr const char* NICKNAME_PARAMETER = ":nickname";
constexpr const char* SCORE_PARAMETER = ":score";
constexpr const char* DEATHS_PARAMETER = ":deaths";
constexpr const char* PLAYED_AT_PARAMETER = ":played_at";
constexpr const char* DIFFICULTY_PARAMETER = ":difficulty";
constexpr const char* PLAYER_COUNT_PARAMETER = ":player_count";
constexpr const char* ASKER_PARAMETER = ":asker";
constexpr const char* TOP_ROWS_PARAMETER = ":top_rows";

/** @brief The columns LEADERBOARD_QUERY gives. */
constexpr const char* BOARD_RANK_COLUMN = "board_rank";
constexpr const char* NICKNAME_COLUMN = "nickname";
constexpr const char* SCORE_COLUMN = "score";
constexpr const char* PLAYED_AT_COLUMN = "played_at";
constexpr const char* IS_ASKER_COLUMN = "is_asker";

}  // namespace rtype::server
