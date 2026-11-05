#include "StatementParts.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include "EpochSeconds.hpp"
#include "MasterMessageText.hpp"
#include "PageRequest.hpp"
#include "ServerStatus.hpp"
#include "ServersRepositoryConstants.hpp"

namespace rtype::master {

void bindPage(SQLite::Statement& statement, const PageRequest& page) {
  statement.bind(std::string{LIMIT_PARAMETER}, page.limit);
  statement.bind(std::string{OFFSET_PARAMETER}, page.offset);
}

void bindMeasure(SQLite::Statement& statement, std::string_view parameter,
                 std::optional<double> measure) {
  if (measure.has_value()) {
    statement.bind(std::string{parameter}, *measure);
  } else {
    statement.bind(std::string{parameter});
  }
}

std::optional<double> measureIn(const SQLite::Column& column) {
  if (column.isNull()) {
    return std::nullopt;
  }
  return column.getDouble();
}

std::optional<std::chrono::sys_seconds> optionalDateIn(
    const SQLite::Column& column) {
  if (column.isNull()) {
    return std::nullopt;
  }
  return dateOfEpochSeconds(column.getInt64());
}

network::ServerStatus statusIn(const SQLite::Column& column) {
  return network::serverStatusOf(column.getString())
      .value_or(network::ServerStatus::Offline);
}

void eraseOlderThan(SQLite::Database& connection, std::string_view statement,
                    std::chrono::sys_seconds oldest) {
  SQLite::Statement erase{connection, std::string{statement}};
  erase.bind(std::string{OLDEST_PARAMETER}, epochSecondsOf(oldest));
  erase.exec();
}

}  // namespace rtype::master
