#include "HeartbeatSampleRepository.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "EpochSeconds.hpp"
#include "HeartbeatRecord.hpp"
#include "MasterDatabase.hpp"
#include "PageRequest.hpp"
#include "PlayerCount.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerMeasures.hpp"
#include "ServerSample.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StatementParts.hpp"

namespace rtype::master {

namespace {

void bindMeasure(SQLite::Statement& statement, std::string_view parameter,
                 std::optional<double> measure) {
  if (measure.has_value()) {
    statement.bind(std::string{parameter}, *measure);
  } else {
    statement.bind(std::string{parameter});
  }
}

void insertSample(SQLite::Database& connection, const HeartbeatRecord& record) {
  const network::ServerMeasures& measures = record.heartbeat.measures;
  SQLite::Statement insert{connection, std::string{INSERT_SAMPLE_STATEMENT}};
  insert.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
  insert.bind(std::string{TAKEN_AT_PARAMETER},
              epochSecondsOf(record.receivedAt));
  insert.bind(std::string{GAME_COUNT_PARAMETER},
              static_cast<std::int64_t>(record.heartbeat.games.size()));
  insert.bind(std::string{PLAYER_COUNT_PARAMETER},
              playerCountOf(record.heartbeat));
  bindMeasure(insert, TICK_P50_PARAMETER, measures.tickP50Milliseconds);
  bindMeasure(insert, TICK_P95_PARAMETER, measures.tickP95Milliseconds);
  bindMeasure(insert, RECEIVED_BYTES_PARAMETER,
              measures.receivedBytesPerSecond);
  bindMeasure(insert, SENT_BYTES_PARAMETER, measures.sentBytesPerSecond);
  bindMeasure(insert, LOSS_PERCENT_PARAMETER, measures.lossPercent);
  bindMeasure(insert, AVERAGE_PING_PARAMETER, measures.averagePingMilliseconds);
  insert.exec();
}

void eraseOldSamples(SQLite::Database& connection,
                     std::chrono::sys_seconds now) {
  SQLite::Statement erase{connection, std::string{ERASE_OLD_SAMPLES_STATEMENT}};
  erase.bind(std::string{OLDEST_PARAMETER},
             epochSecondsOf(now - SAMPLE_RETENTION));
  erase.exec();
}

/** @returns The measure @p column holds, empty while it is null. */
std::optional<double> measureIn(const SQLite::Column& column) {
  if (column.isNull()) {
    return std::nullopt;
  }
  return column.getDouble();
}

network::ServerSample sampleIn(const SQLite::Statement& query) {
  return network::ServerSample{
      .takenAt = dateOfEpochSeconds(
          query.getColumn(SAMPLE_TAKEN_AT_COLUMN).getInt64()),
      .gameCount = static_cast<std::uint16_t>(
          query.getColumn(SAMPLE_GAME_COUNT_COLUMN).getUInt()),
      .playerCount = static_cast<std::uint16_t>(
          query.getColumn(SAMPLE_PLAYER_COUNT_COLUMN).getUInt()),
      .measures = network::ServerMeasures{
          .tickP50Milliseconds =
              measureIn(query.getColumn(SAMPLE_TICK_P50_COLUMN)),
          .tickP95Milliseconds =
              measureIn(query.getColumn(SAMPLE_TICK_P95_COLUMN)),
          .receivedBytesPerSecond =
              measureIn(query.getColumn(SAMPLE_RECEIVED_BYTES_COLUMN)),
          .sentBytesPerSecond =
              measureIn(query.getColumn(SAMPLE_SENT_BYTES_COLUMN)),
          .lossPercent = measureIn(query.getColumn(SAMPLE_LOSS_PERCENT_COLUMN)),
          .averagePingMilliseconds =
              measureIn(query.getColumn(SAMPLE_AVERAGE_PING_COLUMN))}};
}

}  // namespace

HeartbeatSampleRepository::HeartbeatSampleRepository(MasterDatabase& database)
    : database_(database) {}

void HeartbeatSampleRepository::add(const HeartbeatRecord& record) {
  database_.run([&record](SQLite::Database& connection) {
    SQLite::Transaction transaction{connection,
                                    SQLite::TransactionBehavior::IMMEDIATE};
    insertSample(connection, record);
    eraseOldSamples(connection, record.receivedAt);
    transaction.commit();
  });
}

std::vector<network::ServerSample> HeartbeatSampleRepository::samplesOf(
    std::int64_t serverId, const PageRequest& page) {
  return database_.run([serverId, &page](SQLite::Database& connection) {
    SQLite::Statement query{connection, std::string{SAMPLES_OF_SERVER_QUERY}};
    query.bind(std::string{SERVER_ID_PARAMETER}, serverId);
    bindPage(query, page);
    return rowsOf(query, sampleIn);
  });
}

}  // namespace rtype::master
