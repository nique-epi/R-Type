#include "StatusChangeRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <cstdint>
#include <string>
#include <vector>
#include "EpochSeconds.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "PageRequest.hpp"
#include "ServerStatusChange.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StatementParts.hpp"
#include "StatusChangeRecord.hpp"

namespace rtype::master {

namespace {

void insertChange(SQLite::Database& connection,
                  const StatusChangeRecord& record) {
  SQLite::Statement insert{connection,
                           std::string{INSERT_STATUS_CHANGE_STATEMENT}};
  insert.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
  insert.bind(std::string{CHANGED_AT_PARAMETER},
              epochSecondsOf(record.change.changedAt));
  insert.bind(std::string{STATUS_PARAMETER},
              std::string{network::serverStatusText(record.change.status)});
  insert.exec();
}

network::ServerStatusChange changeIn(const SQLite::Statement& query) {
  return network::ServerStatusChange{
      .changedAt =
          dateOfEpochSeconds(query.getColumn(CHANGED_AT_COLUMN).getInt64()),
      .status = statusIn(query.getColumn(CHANGE_STATUS_COLUMN))};
}

}  // namespace

StatusChangeRepository::StatusChangeRepository(MasterDatabase& database)
    : database_(database) {}

void StatusChangeRepository::add(
    const std::vector<StatusChangeRecord>& changes) {
  if (changes.empty()) {
    return;
  }
  database_.run([&changes](SQLite::Database& connection) {
    SQLite::Transaction transaction{connection,
                                    SQLite::TransactionBehavior::IMMEDIATE};
    for (const StatusChangeRecord& change : changes) {
      insertChange(connection, change);
    }
    eraseOlderThan(connection, ERASE_OLD_STATUS_CHANGES_STATEMENT,
                   changes.front().change.changedAt - STATUS_CHANGE_RETENTION);
    transaction.commit();
  });
}

std::vector<network::ServerStatusChange> StatusChangeRepository::changesOf(
    std::int64_t serverId, const PageRequest& page) {
  return database_.run([serverId, &page](SQLite::Database& connection) {
    SQLite::Statement query{connection,
                            std::string{STATUS_CHANGES_OF_SERVER_QUERY}};
    query.bind(std::string{SERVER_ID_PARAMETER}, serverId);
    bindPage(query, page);
    return rowsOf(query, changeIn);
  });
}

}  // namespace rtype::master
