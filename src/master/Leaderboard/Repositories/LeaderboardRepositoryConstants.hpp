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
  WHERE difficulty = :difficulty AND player_count = :player_count
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

/** @brief The parameters of those statements, beside the :server_id,
 * :player_count, :limit and :offset of ServersRepositoryConstants.hpp. */
constexpr std::string_view GAME_ID_PARAMETER = ":game_id";
constexpr std::string_view NICKNAME_PARAMETER = ":nickname";
constexpr std::string_view ACCOUNT_ID_PARAMETER = ":account_id";
constexpr std::string_view SCORE_PARAMETER = ":score";
constexpr std::string_view PLAYED_AT_PARAMETER = ":played_at";
constexpr std::string_view DIFFICULTY_PARAMETER = ":difficulty";
constexpr std::string_view PLAYER_PARAMETER = ":player";

/** @brief The columns of LEADERBOARD_QUERY. */
constexpr int BOARD_RANK_COLUMN = 0;
constexpr int NICKNAME_COLUMN = 1;
constexpr int SCORE_COLUMN = 2;
constexpr int PLAYED_AT_COLUMN = 3;
constexpr int BOARD_SERVER_ID_COLUMN = 4;
constexpr int SERVER_NAME_COLUMN = 5;
constexpr int IN_PAGE_COLUMN = 6;
constexpr int NAMED_COLUMN = 7;

}  // namespace rtype::master
