#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief The first version of the file of a server.
 *
 * A nickname compares with NOCASE, which folds ASCII letters only, as
 * PlayerRoster::hasNickname() does: ACE and ace are one player. account_id
 * stays empty: a server knows no account, only the game ticket a player
 * hands it (see RESULTS_FOR_THE_WALLETS_MIGRATION); sent_at is the
 * second the master accepted the game (see ResultsDatabase::markSent());
 * vanilla is 1 for a vanilla game (GameResult::vanilla). Statistics are
 * computed from the rows, so they cannot disagree with them; COALESCE gives no
 * collation of its own, hence the explicit one.
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

/**
 * @brief The second version: refused_at, the second the master refused a game
 * for good (a game it never keeps, see ResultsDatabase::markRefused()), and an
 * index of the vanilla games the master has not answered, read after each
 * heartbeat.
 */
constexpr std::string_view UNSENT_GAMES_MIGRATION = R"sql(
ALTER TABLE game_results ADD COLUMN refused_at INTEGER;

CREATE INDEX game_results_not_sent
  ON game_results (played_at, game_id)
  WHERE vanilla = 1 AND sent_at IS NULL AND refused_at IS NULL;
)sql";

/**
 * @brief The fifth version: won_looks, the names of the look items the player
 * won in the game, separated by WON_LOOKS_SEPARATOR, empty when it won none;
 * victory, 1 when the players cleared the level; ticket, the game ticket the
 * player joined with as the master writes it, empty for a guest, which the
 * server only reports to the master with the game and never reads in its
 * leaderboard or its statistics. A row written before this version reads as
 * a game of a guest, lost without any item won.
 */
constexpr std::string_view RESULTS_FOR_THE_WALLETS_MIGRATION = R"sql(
ALTER TABLE game_results ADD COLUMN won_looks TEXT NOT NULL DEFAULT '';
ALTER TABLE game_results ADD COLUMN victory INTEGER NOT NULL DEFAULT 0;
ALTER TABLE game_results ADD COLUMN ticket TEXT;
)sql";

/**
 * @brief The sixth version: bounty, the kopecks the bosses destroyed and the
 * level cleared while the player was there paid it, which the server reports
 * to the master with the game. A row written before this version reads as no
 * bounty.
 */
constexpr std::string_view RESULTS_WITH_THE_BOUNTIES_MIGRATION =
    "ALTER TABLE game_results ADD COLUMN bounty INTEGER NOT NULL DEFAULT 0;";

/** @brief What separates two names of won_looks: a name of
 * identifiers::SHIP_LOOK_ITEM_NAMES holds none. */
constexpr char WON_LOOKS_SEPARATOR = ' ';

/** @brief Draws the identifier of a game, as the SQLite documentation advises
 * for a globally unique identifier. */
constexpr std::string_view NEW_GAME_ID_QUERY =
    "SELECT lower(hex(randomblob(16)))";

constexpr std::string_view INSERT_RESULT_STATEMENT = R"sql(
INSERT INTO game_results
  (game_id, nickname, ticket, score, bounty, deaths, played_at, difficulty,
   player_count, vanilla, won_looks, victory)
VALUES
  (:game_id, :nickname, :ticket, :score, :bounty, :deaths, :played_at,
   :difficulty, :player_count, :vanilla, :won_looks, :victory)
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
  WHERE (:difficulty IS NULL OR difficulty = :difficulty)
    AND (:player_count IS NULL OR player_count = :player_count)
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

/**
 * @brief Every row of the gameLimit oldest vanilla games the master has not
 * answered, a game after the other, the oldest first. Every row of a game
 * holds its played_at, so DISTINCT finds the games without a GROUP BY, and
 * SQLite walks game_results_not_sent in its order and stops after gameLimit
 * games, statistics or not; its WHERE must stay the one of that index.
 */
constexpr std::string_view UNSENT_GAMES_QUERY = R"sql(
WITH waiting AS (
  SELECT DISTINCT played_at, game_id
  FROM game_results
  WHERE vanilla = 1 AND sent_at IS NULL AND refused_at IS NULL
  ORDER BY played_at, game_id
  LIMIT :game_limit
)
SELECT results.game_id, results.nickname, results.ticket, results.score,
       results.bounty, results.deaths, results.played_at, results.difficulty,
       results.player_count, results.won_looks, results.victory
FROM waiting
JOIN game_results AS results ON results.game_id = waiting.game_id
ORDER BY waiting.played_at, waiting.game_id, results.nickname
)sql";

/** @brief Write when the master answered a game, on each of its rows. */
constexpr std::string_view MARK_SENT_STATEMENT =
    "UPDATE game_results SET sent_at = :answered_at WHERE game_id = :game_id";
constexpr std::string_view MARK_REFUSED_STATEMENT =
    "UPDATE game_results SET refused_at = :answered_at "
    "WHERE game_id = :game_id";

/** @brief The parameters of the statements above. */
constexpr const char* GAME_ID_PARAMETER = ":game_id";
constexpr const char* NICKNAME_PARAMETER = ":nickname";
constexpr const char* SCORE_PARAMETER = ":score";
constexpr const char* BOUNTY_PARAMETER = ":bounty";
constexpr const char* DEATHS_PARAMETER = ":deaths";
constexpr const char* PLAYED_AT_PARAMETER = ":played_at";
constexpr const char* DIFFICULTY_PARAMETER = ":difficulty";
constexpr const char* PLAYER_COUNT_PARAMETER = ":player_count";
constexpr const char* VANILLA_PARAMETER = ":vanilla";
constexpr const char* TICKET_PARAMETER = ":ticket";
constexpr const char* WON_LOOKS_PARAMETER = ":won_looks";
constexpr const char* VICTORY_PARAMETER = ":victory";
constexpr const char* ASKER_PARAMETER = ":asker";
constexpr const char* TOP_ROWS_PARAMETER = ":top_rows";
constexpr const char* GAME_LIMIT_PARAMETER = ":game_limit";
constexpr const char* ANSWERED_AT_PARAMETER = ":answered_at";

/** @brief The columns LEADERBOARD_QUERY gives. */
constexpr const char* BOARD_RANK_COLUMN = "board_rank";
constexpr const char* NICKNAME_COLUMN = "nickname";
constexpr const char* SCORE_COLUMN = "score";
constexpr const char* PLAYED_AT_COLUMN = "played_at";
constexpr const char* IS_ASKER_COLUMN = "is_asker";

/** @brief The columns UNSENT_GAMES_QUERY gives besides those above. */
constexpr const char* GAME_ID_COLUMN = "game_id";
constexpr const char* DEATHS_COLUMN = "deaths";
constexpr const char* BOUNTY_COLUMN = "bounty";
constexpr const char* DIFFICULTY_COLUMN = "difficulty";
constexpr const char* PLAYER_COUNT_COLUMN = "player_count";
constexpr const char* TICKET_COLUMN = "ticket";
constexpr const char* WON_LOOKS_COLUMN = "won_looks";
constexpr const char* VICTORY_COLUMN = "victory";

}  // namespace rtype::server
