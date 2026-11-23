#pragma once

#include <string_view>

namespace rtype::master {

/** @brief Whether the master keeps a row of the game :game_id of the server
 * :server_id: one row is enough, since a game is kept whole. */
constexpr std::string_view GAME_KEPT_QUERY = R"sql(
SELECT 1 FROM game_results
WHERE server_id = :server_id AND game_id = :game_id
LIMIT 1
)sql";

/** @brief Adds the row of one player of a game the master does not keep. */
constexpr std::string_view INSERT_RESULT_STATEMENT = R"sql(
INSERT INTO game_results
  (server_id, game_id, nickname, account_id, score, played_at, difficulty,
   player_count)
VALUES
  (:server_id, :game_id, :nickname, :account_id, :score, :played_at,
   :difficulty, :player_count)
)sql";

/**
 * @brief The best row of each player of one leaderboard, the best score first.
 *
 * A player is an account, under any nickname, its id compared as written; or a
 * guest, one per nickname compared with NOCASE (ASCII letters folded only),
 * whatever its server. The window `pilot` holds the rows of one player. Among
 * them, the earliest of the best scores counts.
 *
 * board_rank gives equal scores one rank; board_place orders them, the
 * earliest game first, then by nickname and account, so a page is the same at
 * each read. A row is given when it is in the page (in_page), or when it is
 * the best row of a player who played under the nickname :player in any game
 * of this leaderboard (named, never for a null :player). A null
 * :server_id reads every server.
 */
constexpr std::string_view LEADERBOARD_QUERY = R"sql(
WITH best AS (
  SELECT server_id, nickname, account_id, score, played_at,
         ROW_NUMBER() OVER (
           pilot ORDER BY score DESC, played_at ASC, server_id ASC)
           AS position,
         MAX(COALESCE(nickname = :player COLLATE NOCASE, 0)) OVER pilot
           AS named
  FROM game_results
  WHERE (:difficulty IS NULL OR difficulty = :difficulty)
    AND (:player_count IS NULL OR player_count = :player_count)
    AND (:server_id IS NULL OR server_id = :server_id)
  WINDOW pilot AS (
    PARTITION BY account_id,
                 CASE WHEN account_id IS NULL THEN nickname END COLLATE NOCASE)
),
ranked AS (
  SELECT server_id, nickname, score, played_at, named,
         RANK() OVER (ORDER BY score DESC) AS board_rank,
         ROW_NUMBER() OVER (
           ORDER BY score DESC, played_at ASC, nickname ASC, account_id ASC)
           AS board_place
  FROM best
  WHERE position = 1
),
marked AS (
  SELECT ranked.*,
         board_place > :offset AND board_place <= :offset + :limit
           AS in_page
  FROM ranked
)
SELECT marked.board_rank, marked.nickname, marked.score, marked.played_at,
       marked.server_id, servers.name, marked.in_page, marked.named
FROM marked JOIN servers ON servers.id = marked.server_id
WHERE marked.in_page OR marked.named
ORDER BY marked.board_place
)sql";

/**
 * @brief The total of the scores of each account, the highest first, a page
 * at a time: every difficulty, number of players and server together.
 *
 * A game counts for the account its ticket paid: the row of credited_results
 * whose reference names the row of game_results, :reference_prefix, the
 * server, :reference_separator, the game, :reference_separator and the
 * nickname, as LeaderboardService writes it. A guest, and a player whose
 * ticket paid nothing, count for no account. The account is named by its own
 * nickname, whatever nickname it played under.
 *
 * board_rank gives equal totals one rank; board_place orders them, the
 * account whose last game is the earliest first, then by id, so a page is the
 * same at each read. Each row of game_results looks its reference up by the
 * key of credited_results.
 */
constexpr std::string_view ACCOUNT_LEADERBOARD_QUERY = R"sql(
WITH totals AS (
  SELECT credited_results.user_id AS user_id,
         SUM(game_results.score) AS score,
         MAX(game_results.played_at) AS last_played_at
  FROM game_results
  JOIN credited_results
    ON credited_results.reference =
       :reference_prefix || game_results.server_id || :reference_separator
       || game_results.game_id || :reference_separator
       || game_results.nickname
  GROUP BY credited_results.user_id
),
ranked AS (
  SELECT user_id, score, last_played_at,
         RANK() OVER (ORDER BY score DESC) AS board_rank,
         ROW_NUMBER() OVER (
           ORDER BY score DESC, last_played_at ASC, user_id ASC)
           AS board_place
  FROM totals
)
SELECT ranked.board_rank, users.nickname, ranked.score, ranked.last_played_at
FROM ranked JOIN users ON users.id = ranked.user_id
WHERE ranked.board_place > :offset AND ranked.board_place <= :offset + :limit
ORDER BY ranked.board_place
)sql";

/** @brief The parameters of those statements, beside the :server_id,
 * :player_count, :limit and :offset of ServersRepositoryConstants.hpp. */
constexpr std::string_view GAME_ID_PARAMETER = ":game_id";
constexpr std::string_view NICKNAME_PARAMETER = ":nickname";
constexpr std::string_view ACCOUNT_ID_PARAMETER = ":account_id";
constexpr std::string_view SCORE_PARAMETER = ":score";
constexpr std::string_view PLAYED_AT_PARAMETER = ":played_at";
constexpr std::string_view DIFFICULTY_PARAMETER = ":difficulty";
constexpr std::string_view PLAYER_PARAMETER = ":player";
constexpr std::string_view REFERENCE_PREFIX_PARAMETER = ":reference_prefix";
constexpr std::string_view REFERENCE_SEPARATOR_PARAMETER =
    ":reference_separator";

/** @brief The columns of LEADERBOARD_QUERY; ACCOUNT_LEADERBOARD_QUERY gives
 * the first four, its total as the score and its last game as the date. */
constexpr int BOARD_RANK_COLUMN = 0;
constexpr int NICKNAME_COLUMN = 1;
constexpr int SCORE_COLUMN = 2;
constexpr int PLAYED_AT_COLUMN = 3;
constexpr int BOARD_SERVER_ID_COLUMN = 4;
constexpr int SERVER_NAME_COLUMN = 5;
constexpr int IN_PAGE_COLUMN = 6;
constexpr int NAMED_COLUMN = 7;

}  // namespace rtype::master
