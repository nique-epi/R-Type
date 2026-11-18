#pragma once

#include <string_view>

namespace rtype::master {

/**
 * @brief The statements of the table `device_links`. A short code is the key:
 * a new link whose code another live link holds is not inserted, and the
 * caller draws another code. A decision is written only on a link no one
 * decided yet, in the same step, so two players who answer at once leave one
 * decision. A link read by its secret names its decider in its first three
 * columns, as userIn() reads them.
 */
constexpr std::string_view INSERT_DEVICE_LINK_STATEMENT = R"sql(
INSERT INTO device_links
  (user_code, secret_fingerprint, address, created_at, expires_at, read_at)
VALUES (:user_code, :secret_fingerprint, :address, :created_at, :expires_at,
        :created_at)
ON CONFLICT DO NOTHING
)sql";
constexpr std::string_view DEVICE_LINKS_CREATED_SINCE_QUERY = R"sql(
SELECT COUNT(*) FROM device_links
WHERE address = :address AND created_at > :since
)sql";
constexpr std::string_view FORGET_DEVICE_LINKS_STATEMENT =
    "DELETE FROM device_links WHERE expires_at < :oldest";
constexpr std::string_view DEVICE_LINK_OF_SECRET_QUERY = R"sql(
SELECT users.id, users.nickname, users.role, device_links.user_code,
       device_links.created_at, device_links.expires_at, device_links.read_at,
       device_links.decision
FROM device_links LEFT JOIN users ON users.id = device_links.user_id
WHERE device_links.secret_fingerprint = :secret_fingerprint
)sql";
constexpr std::string_view DEVICE_LINK_OF_USER_CODE_QUERY = R"sql(
SELECT users.id, users.nickname, users.role, device_links.user_code,
       device_links.created_at, device_links.expires_at, device_links.read_at,
       device_links.decision
FROM device_links LEFT JOIN users ON users.id = device_links.user_id
WHERE device_links.user_code = :user_code
)sql";
constexpr std::string_view MARK_DEVICE_LINK_READ_STATEMENT = R"sql(
UPDATE device_links SET read_at = :read_at
WHERE secret_fingerprint = :secret_fingerprint
)sql";
constexpr std::string_view DECIDE_DEVICE_LINK_STATEMENT = R"sql(
UPDATE device_links SET user_id = :user_id, decision = :decision
WHERE user_code = :user_code AND decision IS NULL
)sql";
constexpr std::string_view DELETE_DEVICE_LINK_STATEMENT =
    "DELETE FROM device_links WHERE secret_fingerprint = :secret_fingerprint";

/** @brief The parameters of those statements, besides OLDEST_PARAMETER,
 * which FORGET_DEVICE_LINKS_STATEMENT shares with the servers. */
constexpr std::string_view LINK_USER_CODE_PARAMETER = ":user_code";
constexpr std::string_view LINK_SECRET_FINGERPRINT_PARAMETER =
    ":secret_fingerprint";
constexpr std::string_view LINK_ADDRESS_PARAMETER = ":address";
constexpr std::string_view LINK_CREATED_AT_PARAMETER = ":created_at";
constexpr std::string_view LINK_EXPIRES_AT_PARAMETER = ":expires_at";
constexpr std::string_view LINK_READ_AT_PARAMETER = ":read_at";
constexpr std::string_view LINK_SINCE_PARAMETER = ":since";
constexpr std::string_view LINK_USER_ID_PARAMETER = ":user_id";
constexpr std::string_view LINK_DECISION_PARAMETER = ":decision";

/** @brief The column of DEVICE_LINKS_CREATED_SINCE_QUERY. */
constexpr int LINK_COUNT_COLUMN = 0;

/** @brief The columns of a link read by its secret or its short code, after
 * the three of its decider. */
constexpr int LINK_DECIDER_ID_COLUMN = 0;
constexpr int LINK_USER_CODE_COLUMN = 3;
constexpr int LINK_CREATED_AT_COLUMN = 4;
constexpr int LINK_EXPIRES_AT_COLUMN = 5;
constexpr int LINK_READ_AT_COLUMN = 6;
constexpr int LINK_DECISION_COLUMN = 7;

}  // namespace rtype::master
