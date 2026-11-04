#include "HeartbeatSampleRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "HeartbeatRecord.hpp"
#include "HostedGame.hpp"
#include "MasterDatabase.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerMeasures.hpp"
#include "ServersRepositoryConstants.hpp"

namespace rtype::master {

namespace {

std::int64_t secondsOf(std::chrono::sys_seconds date) {
  return static_cast<std::int64_t>(date.time_since_epoch().count());
}

void bindMeasure(SQLite::Statement& statement, std::string_view parameter,
                 std::optional<double> measure) {
  if (measure.has_value()) {
    statement.bind(std::string{parameter}, *measure);
  } else {
    statement.bind(std::string{parameter});
  }
}

std::int64_t playerCountOf(const network::ServerHeartbeat& heartbeat) {
  std::int64_t count = 0;
  for (const network::HostedGame& game : heartbeat.games) {
    count += static_cast<std::int64_t>(game.players.size());
  }
  return count;
}

void insertSample(SQLite::Database& connection, const HeartbeatRecord& record) {
  const network::ServerMeasures& measures = record.heartbeat.measures;
  SQLite::Statement insert{connection, std::string{INSERT_SAMPLE_STATEMENT}};
  insert.bind(std::string{SERVER_ID_PARAMETER}, record.serverId);
  insert.bind(std::string{TAKEN_AT_PARAMETER}, secondsOf(record.receivedAt));
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
  erase.bind(std::string{OLDEST_PARAMETER}, secondsOf(now - SAMPLE_RETENTION));
  erase.exec();
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

}  // namespace rtype::master
