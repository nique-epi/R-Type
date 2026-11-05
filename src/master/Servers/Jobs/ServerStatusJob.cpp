#include "ServerStatusJob.hpp"
#include <chrono>
#include <exception>
#include <stop_token>
#include <string>
#include <vector>
#include "InterruptibleSleep.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterMessageText.hpp"
#include "StatusChangeRecord.hpp"

namespace rtype::master {

ServerStatusJob::ServerStatusJob(MasterDatabase& database,
                                 std::chrono::milliseconds interval)
    : service_(database),
      interval_(interval),
      logger_(std::string{MASTER_LOGGER_NAME}),
      statusWatch_([this](const std::stop_token& stop) { run(stop); }) {}

void ServerStatusJob::run(const std::stop_token& stop) {
  while (!stop.stop_requested()) {
    tell(refresh());
    concurrency::sleepUnlessStopped(stop, interval_);
  }
}

std::vector<StatusChangeRecord> ServerStatusJob::refresh() {
  try {
    return service_.refreshStatuses(std::chrono::floor<std::chrono::seconds>(
        std::chrono::system_clock::now()));
  } catch (const std::exception& error) {
    logger_.error("the statuses of the servers were not refreshed: ",
                  error.what());
    return {};
  }
}

void ServerStatusJob::tell(
    const std::vector<StatusChangeRecord>& changes) const {
  for (const StatusChangeRecord& change : changes) {
    logger_.info("server ", change.serverId, " is now ",
                 network::serverStatusText(change.change.status));
  }
}

}  // namespace rtype::master
