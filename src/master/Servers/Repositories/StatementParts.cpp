#include "StatementParts.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Statement.h>
#include <optional>
#include <string>
#include "MasterMessageText.hpp"
#include "NetworkException.hpp"
#include "PageRequest.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerStatus.hpp"
#include "ServersRepositoryConstants.hpp"

namespace rtype::master {

void bindPage(SQLite::Statement& statement, const PageRequest& page) {
  statement.bind(std::string{LIMIT_PARAMETER}, page.limit);
  statement.bind(std::string{OFFSET_PARAMETER}, page.offset);
}

network::ServerStatus statusIn(const SQLite::Column& column) {
  return network::serverStatusOf(column.getString())
      .value_or(network::ServerStatus::Offline);
}

std::optional<network::ServerHeartbeat> readableHeartbeatIn(
    const SQLite::Column& column) {
  if (column.isNull()) {
    return std::nullopt;
  }
  try {
    return network::heartbeatFromText(column.getString());
  } catch (const network::InvalidMasterMessageException&) {
    return std::nullopt;
  }
}

}  // namespace rtype::master
