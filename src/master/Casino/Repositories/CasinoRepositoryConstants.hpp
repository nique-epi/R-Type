#pragma once

#include <string_view>

namespace rtype::master {

/** @brief The statements of the table `casino_seats`. A seat counts while its
 * seen_at is at or after :since. */
constexpr std::string_view SEAT_SEEN_QUERY =
    "SELECT seen_at FROM casino_seats WHERE room = :room AND user_id = "
    ":user_id";
constexpr std::string_view OTHER_SEATS_COUNT_QUERY = R"sql(
SELECT COUNT(*) FROM casino_seats
WHERE room = :room AND user_id != :user_id AND seen_at >= :since
)sql";
constexpr std::string_view REPLACE_SEAT_STATEMENT = R"sql(
INSERT INTO casino_seats (room, user_id, seen_at)
VALUES (:room, :user_id, :seen_at)
ON CONFLICT (room, user_id) DO UPDATE SET seen_at = excluded.seen_at
)sql";
constexpr std::string_view DELETE_SEAT_STATEMENT =
    "DELETE FROM casino_seats WHERE room = :room AND user_id = :user_id";
constexpr std::string_view SEATED_PLAYERS_QUERY = R"sql(
SELECT casino_seats.user_id, users.nickname FROM casino_seats
JOIN users ON users.id = casino_seats.user_id
WHERE casino_seats.room = :room AND casino_seats.seen_at >= :since
ORDER BY casino_seats.seen_at, casino_seats.user_id
)sql";
constexpr std::string_view SEATED_COUNTS_QUERY = R"sql(
SELECT room, COUNT(*) FROM casino_seats WHERE seen_at >= :since
GROUP BY room
)sql";
constexpr std::string_view DELETE_OLD_SEATS_STATEMENT =
    "DELETE FROM casino_seats WHERE seen_at < :since";

/** @brief The statements of the table `casino_bets`. */
constexpr std::string_view PLAYER_BETS_QUERY = R"sql(
SELECT kind, first_number, amount FROM casino_bets
WHERE room = :room AND round = :round AND user_id = :user_id
ORDER BY position
)sql";
constexpr std::string_view ROUND_BETS_QUERY = R"sql(
SELECT user_id, kind, first_number, amount FROM casino_bets
WHERE room = :room AND round = :round
ORDER BY user_id, position
)sql";
constexpr std::string_view DELETE_PLAYER_BETS_STATEMENT = R"sql(
DELETE FROM casino_bets
WHERE room = :room AND round = :round AND user_id = :user_id
)sql";
constexpr std::string_view INSERT_BET_STATEMENT = R"sql(
INSERT INTO casino_bets
  (room, round, user_id, position, kind, first_number, amount)
VALUES (:room, :round, :user_id, :position, :kind, :first_number, :amount)
)sql";
constexpr std::string_view UNDRAWN_ROUNDS_QUERY = R"sql(
SELECT DISTINCT room, round FROM casino_bets
WHERE round < :round
  AND NOT EXISTS (SELECT 1 FROM casino_rounds
                  WHERE casino_rounds.room = casino_bets.room
                    AND casino_rounds.round = casino_bets.round)
ORDER BY round, room
)sql";
constexpr std::string_view DELETE_SETTLED_BETS_STATEMENT = R"sql(
DELETE FROM casino_bets
WHERE EXISTS (SELECT 1 FROM casino_rounds
              WHERE casino_rounds.room = casino_bets.room
                AND casino_rounds.round = casino_bets.round
                AND casino_rounds.drawn_at < :before)
)sql";

/** @brief The statements of the table `casino_rounds`. A round is kept while
 * bets name it, or while it is one of the last :kept rounds of its room. */
constexpr std::string_view WINNING_NUMBER_QUERY = R"sql(
SELECT winning_number FROM casino_rounds WHERE room = :room AND round = :round
)sql";
constexpr std::string_view INSERT_ROUND_STATEMENT = R"sql(
INSERT INTO casino_rounds (room, round, winning_number, drawn_at)
VALUES (:room, :round, :winning_number, :drawn_at)
ON CONFLICT (room, round) DO NOTHING
)sql";
constexpr std::string_view HISTORY_QUERY = R"sql(
SELECT winning_number FROM casino_rounds WHERE room = :room AND round < :round
ORDER BY round DESC LIMIT :kept
)sql";
constexpr std::string_view PRUNE_ROUNDS_STATEMENT = R"sql(
DELETE FROM casino_rounds
WHERE round <= (SELECT MAX(newest.round) FROM casino_rounds AS newest
                WHERE newest.room = casino_rounds.room) - :kept
  AND NOT EXISTS (SELECT 1 FROM casino_bets
                  WHERE casino_bets.room = casino_rounds.room
                    AND casino_bets.round = casino_rounds.round)
)sql";

/** @brief The parameters of the statements of the casino. */
constexpr std::string_view ROOM_PARAMETER = ":room";
constexpr std::string_view ROUND_PARAMETER = ":round";
constexpr std::string_view CASINO_USER_ID_PARAMETER = ":user_id";
constexpr std::string_view SEEN_SINCE_PARAMETER = ":since";
constexpr std::string_view SEEN_AT_PARAMETER = ":seen_at";
constexpr std::string_view POSITION_PARAMETER = ":position";
constexpr std::string_view KIND_PARAMETER = ":kind";
constexpr std::string_view FIRST_NUMBER_PARAMETER = ":first_number";
constexpr std::string_view BET_AMOUNT_PARAMETER = ":amount";
constexpr std::string_view WINNING_NUMBER_PARAMETER = ":winning_number";
constexpr std::string_view DRAWN_AT_PARAMETER = ":drawn_at";
constexpr std::string_view BEFORE_PARAMETER = ":before";
constexpr std::string_view KEPT_PARAMETER = ":kept";

/** @brief The columns the queries of the casino read. */
constexpr int PLAYER_BET_KIND_COLUMN = 0;
constexpr int ROUND_BET_USER_COLUMN = 0;
constexpr int ROUND_BET_KIND_COLUMN = 1;

}  // namespace rtype::master
