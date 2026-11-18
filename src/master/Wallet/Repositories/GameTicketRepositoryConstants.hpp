#pragma once

#include <string_view>

namespace rtype::master {

/** @brief The statements of the table `game_tickets`. A ticket is redeemed
 * once: the update that marks it used names the account in the same step, so
 * two results that present one ticket at once pay one of them. */
constexpr std::string_view INSERT_GAME_TICKET_STATEMENT = R"sql(
INSERT INTO game_tickets (fingerprint, user_id, issued_at, expires_at)
VALUES (:fingerprint, :user_id, :issued_at, :expires_at)
)sql";
constexpr std::string_view TICKETS_ISSUED_SINCE_QUERY = R"sql(
SELECT COUNT(*) FROM game_tickets
WHERE user_id = :user_id AND issued_at > :since
)sql";
constexpr std::string_view FORGET_EXPIRED_TICKETS_STATEMENT =
    "DELETE FROM game_tickets WHERE expires_at < :now";
constexpr std::string_view OWN_TICKET_QUERY = R"sql(
SELECT expires_at, used_at IS NOT NULL FROM game_tickets
WHERE fingerprint = :fingerprint AND user_id = :user_id
)sql";
constexpr std::string_view REDEEM_TICKET_STATEMENT = R"sql(
UPDATE game_tickets SET used_at = :now
WHERE fingerprint = :fingerprint AND used_at IS NULL AND expires_at >= :now
RETURNING user_id
)sql";

/** @brief The parameters of those statements. */
constexpr std::string_view TICKET_FINGERPRINT_PARAMETER = ":fingerprint";
constexpr std::string_view TICKET_USER_ID_PARAMETER = ":user_id";
constexpr std::string_view TICKET_ISSUED_AT_PARAMETER = ":issued_at";
constexpr std::string_view TICKET_EXPIRES_AT_PARAMETER = ":expires_at";
constexpr std::string_view TICKET_SINCE_PARAMETER = ":since";
constexpr std::string_view TICKET_NOW_PARAMETER = ":now";

/** @brief The column of TICKETS_ISSUED_SINCE_QUERY and of
 * REDEEM_TICKET_STATEMENT. */
constexpr int TICKET_COUNT_COLUMN = 0;
constexpr int TICKET_OWNER_COLUMN = 0;

/** @brief The columns of OWN_TICKET_QUERY. */
constexpr int TICKET_EXPIRES_AT_COLUMN = 0;
constexpr int TICKET_USED_COLUMN = 1;

}  // namespace rtype::master
