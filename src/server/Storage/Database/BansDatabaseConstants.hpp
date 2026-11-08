#pragma once

#include <string_view>

namespace rtype::server {

/**
 * @brief The third version of the file of a server: one row per target. A
 * nickname compares with NOCASE, which folds ASCII letters only, as
 * network::nicknameKey() does: a ban of ACE is a ban of ace. expires_at, in
 * seconds since 1970 UTC, is NULL for a ban for good.
 */
constexpr std::string_view BANS_TABLE_MIGRATION = R"sql(
CREATE TABLE bans (
  kind       TEXT    NOT NULL CHECK (kind IN ('nickname', 'address')),
  target     TEXT    NOT NULL COLLATE NOCASE,
  banned_at  INTEGER NOT NULL,
  expires_at INTEGER,
  PRIMARY KEY (kind, target)
) STRICT;
)sql";

/** @brief Writes a ban, in place of the ban of its target, whose text then
 * takes the case the new ban gives it. */
constexpr std::string_view SAVE_BAN_STATEMENT = R"sql(
INSERT INTO bans (kind, target, banned_at, expires_at)
  VALUES (:kind, :target, :banned_at, :expires_at)
  ON CONFLICT (kind, target) DO UPDATE
    SET target = excluded.target, banned_at = excluded.banned_at,
        expires_at = excluded.expires_at
)sql";

constexpr std::string_view REMOVE_BAN_STATEMENT =
    "DELETE FROM bans WHERE kind = :kind AND target = :target";

/** @brief The bans that have not ended at :now, oldest first. */
constexpr std::string_view BANS_IN_FORCE_QUERY = R"sql(
SELECT kind, target, banned_at, expires_at FROM bans
  WHERE expires_at IS NULL OR expires_at > :now
  ORDER BY banned_at, target
)sql";

/** @brief The texts of the kinds, as the CHECK of the table admits them. */
constexpr std::string_view NICKNAME_KIND_TEXT = "nickname";
constexpr std::string_view ADDRESS_KIND_TEXT = "address";

constexpr const char* KIND_PARAMETER = ":kind";
constexpr const char* TARGET_PARAMETER = ":target";
constexpr const char* BANNED_AT_PARAMETER = ":banned_at";
constexpr const char* EXPIRES_AT_PARAMETER = ":expires_at";
constexpr const char* NOW_PARAMETER = ":now";

/** @brief The columns of BANS_IN_FORCE_QUERY, in its order. */
constexpr int KIND_COLUMN = 0;
constexpr int TARGET_COLUMN = 1;
constexpr int BANNED_AT_COLUMN = 2;
constexpr int EXPIRES_AT_COLUMN = 3;

}  // namespace rtype::server
