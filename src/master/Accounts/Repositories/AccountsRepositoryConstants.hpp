#pragma once

#include <string_view>

namespace rtype::master {

/** @brief The statements of the table `accounts`. */
constexpr std::string_view CREATIONS_AND_NICKNAME_QUERY = R"sql(
SELECT
  (SELECT COUNT(*) FROM accounts
   WHERE created_by_address = :created_by_address AND created_at >= :since),
  EXISTS (SELECT 1 FROM accounts WHERE nickname = :nickname)
)sql";
constexpr std::string_view ACCOUNT_CREATIONS_SINCE_QUERY = R"sql(
SELECT COUNT(*) FROM accounts
WHERE created_by_address = :created_by_address AND created_at >= :since
)sql";
constexpr std::string_view INSERT_ACCOUNT_STATEMENT = R"sql(
INSERT INTO accounts
  (nickname, password_hash, role, created_by_address, created_at)
VALUES (:nickname, :password_hash, :role, :created_by_address, :created_at)
ON CONFLICT (nickname) DO NOTHING
RETURNING id
)sql";
constexpr std::string_view ACCOUNT_NAMED_QUERY = R"sql(
SELECT id, nickname, role, password_hash FROM accounts
WHERE nickname = :nickname
)sql";
constexpr std::string_view UPDATE_ROLE_STATEMENT =
    "UPDATE accounts SET role = :role WHERE nickname = :nickname";

/** @brief The statements of the table `sessions`. */
constexpr std::string_view DELETE_EXPIRED_SESSIONS_STATEMENT =
    "DELETE FROM sessions WHERE expires_at < :oldest";
constexpr std::string_view INSERT_SESSION_STATEMENT = R"sql(
INSERT INTO sessions
  (token_fingerprint, account_id, address, created_at, expires_at)
VALUES (:token_fingerprint, :account_id, :address, :created_at, :expires_at)
)sql";
constexpr std::string_view SESSION_OWNER_QUERY = R"sql(
SELECT accounts.id, accounts.nickname, accounts.role, sessions.expires_at
FROM sessions JOIN accounts ON accounts.id = sessions.account_id
WHERE sessions.token_fingerprint = :token_fingerprint
  AND sessions.expires_at > :now
)sql";
constexpr std::string_view EXTEND_SESSION_STATEMENT = R"sql(
UPDATE sessions SET expires_at = :expires_at
WHERE token_fingerprint = :token_fingerprint
)sql";
constexpr std::string_view DELETE_SESSION_STATEMENT =
    "DELETE FROM sessions WHERE token_fingerprint = :token_fingerprint";

/** @brief The statements of the table `sign_in_failures`. */
constexpr std::string_view SIGN_IN_FAILURES_SINCE_QUERY = R"sql(
SELECT COUNT(*) FROM sign_in_failures
WHERE address = :address AND failed_at >= :since
)sql";
constexpr std::string_view INSERT_SIGN_IN_FAILURE_STATEMENT = R"sql(
INSERT INTO sign_in_failures (address, failed_at)
VALUES (:address, :created_at)
)sql";
constexpr std::string_view FORGET_SIGN_IN_FAILURES_STATEMENT =
    "DELETE FROM sign_in_failures WHERE failed_at < :oldest";

/**
 * @brief The parameters the statements of the accounts bind, besides those
 * they share with the servers (CREATED_BY_PARAMETER, CREATED_AT_PARAMETER,
 * SINCE_PARAMETER, ADDRESS_PARAMETER, OLDEST_PARAMETER).
 */
constexpr std::string_view ACCOUNT_NICKNAME_PARAMETER = ":nickname";
constexpr std::string_view PASSWORD_HASH_PARAMETER = ":password_hash";
constexpr std::string_view ROLE_PARAMETER = ":role";
constexpr std::string_view TOKEN_FINGERPRINT_PARAMETER = ":token_fingerprint";
constexpr std::string_view SESSION_ACCOUNT_ID_PARAMETER = ":account_id";
constexpr std::string_view EXPIRES_AT_PARAMETER = ":expires_at";
constexpr std::string_view NOW_PARAMETER = ":now";

/** @brief The columns the queries of the accounts read, by position: an
 * account is its id, its nickname and its role, in the first three. */
constexpr int COUNT_COLUMN = 0;
constexpr int NICKNAME_TAKEN_COLUMN = 1;
constexpr int ROW_ACCOUNT_ID_COLUMN = 0;
constexpr int ROW_NICKNAME_COLUMN = 1;
constexpr int ROW_ROLE_COLUMN = 2;
constexpr int PASSWORD_HASH_COLUMN = 3;
constexpr int SESSION_EXPIRES_AT_COLUMN = 3;
constexpr int INSERTED_ACCOUNT_ID_COLUMN = 0;

}  // namespace rtype::master
