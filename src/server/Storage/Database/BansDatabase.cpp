#include "BansDatabase.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "Ban.hpp"
#include "BanKind.hpp"
#include "BanTarget.hpp"
#include "BansDatabaseConstants.hpp"
#include "DatabaseFile.hpp"
#include "PathText.hpp"
#include "ServerMigrationsConstants.hpp"

namespace rtype::server {

namespace {

std::string_view kindText(BanKind kind) {
  switch (kind) {
    case BanKind::Address:
      return ADDRESS_KIND_TEXT;
    case BanKind::Nickname:
      break;
  }
  return NICKNAME_KIND_TEXT;
}

/** @returns The kind @p text names; the CHECK of the table admits no other. */
BanKind kindOf(const std::string& text) {
  return text == ADDRESS_KIND_TEXT ? BanKind::Address : BanKind::Nickname;
}

std::int64_t secondsOf(std::chrono::sys_seconds moment) {
  return static_cast<std::int64_t>(moment.time_since_epoch().count());
}

std::chrono::sys_seconds momentOf(std::int64_t seconds) {
  return std::chrono::sys_seconds{std::chrono::seconds{seconds}};
}

void bindTarget(SQLite::Statement& statement, const BanTarget& target) {
  statement.bind(KIND_PARAMETER, std::string{kindText(target.kind)});
  statement.bind(TARGET_PARAMETER, target.text);
}

Ban banOf(const SQLite::Statement& query) {
  const SQLite::Column end = query.getColumn(EXPIRES_AT_COLUMN);
  return Ban{
      .target =
          BanTarget{.kind = kindOf(query.getColumn(KIND_COLUMN).getString()),
                    .text = query.getColumn(TARGET_COLUMN).getString()},
      .bannedAt = momentOf(query.getColumn(BANNED_AT_COLUMN).getInt64()),
      .expiresAt = end.isNull() ? std::nullopt
                                : std::optional{momentOf(end.getInt64())}};
}

}  // namespace

BansDatabase::BansDatabase(const std::filesystem::path& file)
    : database_(storage::openDatabaseFile(logging::genericText(file),
                                          SERVER_MIGRATIONS)) {}

void BansDatabase::save(const Ban& ban) {
  SQLite::Statement insert{database_, std::string{SAVE_BAN_STATEMENT}};
  bindTarget(insert, ban.target);
  insert.bind(BANNED_AT_PARAMETER, secondsOf(ban.bannedAt));
  if (ban.expiresAt.has_value()) {
    insert.bind(EXPIRES_AT_PARAMETER, secondsOf(*ban.expiresAt));
  } else {
    insert.bind(EXPIRES_AT_PARAMETER);
  }
  insert.exec();
}

bool BansDatabase::remove(const BanTarget& target) {
  SQLite::Statement removal{database_, std::string{REMOVE_BAN_STATEMENT}};
  bindTarget(removal, target);
  return removal.exec() > 0;
}

std::vector<Ban> BansDatabase::inForce(std::chrono::sys_seconds now) const {
  SQLite::Statement query{database_, std::string{BANS_IN_FORCE_QUERY}};
  query.bind(NOW_PARAMETER, secondsOf(now));
  std::vector<Ban> bans;
  while (query.executeStep()) {
    bans.push_back(banOf(query));
  }
  return bans;
}

}  // namespace rtype::server
