#include "HeartbeatJob.hpp"
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <utility>
#include <vector>
#include "HeartbeatJobParts.hpp"
#include "HostedGame.hpp"
#include "MasterMessages.hpp"
#include "MetricsReport.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"
#include "ServerMasterConstants.hpp"

namespace rtype::server {

namespace {

/** @brief Sleeps @p duration, or less when @p stop is requested. */
void pause(const std::stop_token& stop, std::chrono::milliseconds duration) {
  std::mutex mutex;
  std::condition_variable_any never;
  std::unique_lock lock{mutex};
  static_cast<void>(never.wait_for(lock, stop, duration, [] { return false; }));
}

}  // namespace

HeartbeatJob::HeartbeatJob(HeartbeatJobParts parts)
    : client_(std::move(parts.access)),
      identity_(std::move(parts.identity)),
      games_(parts.games),
      metrics_(parts.metrics),
      times_(parts.times),
      logger_(std::string{SERVER_LOGGER_NAME}),
      thread_([this](const std::stop_token& stop) { run(stop); }) {}

void HeartbeatJob::run(const std::stop_token& stop) {
  bool registered = false;
  std::uint64_t seen = 0;
  while (!stop.stop_requested()) {
    if (!registered) {
      registered = registerServer();
      if (!registered) {
        pause(stop, times_.retry);
        continue;
      }
    }
    seen = games_.get().revision();
    registered = sendHeartbeat();
    pause(stop, times_.noticeGap);
    static_cast<void>(games_.get().waitForNewer(
        seen, times_.interval - times_.noticeGap, stop));
  }
}

bool HeartbeatJob::registerServer() {
  try {
    client_.registerServer(registrationOf(identity_));
    logger_.info("registered with the master as ", identity_.name);
    lastProblem_.clear();
    return true;
  } catch (const network::MasterRefusedException& refusal) {
    tell(refusal.what());
  } catch (const network::MasterUnreachableException& silence) {
    tell(silence.what());
  }
  return false;
}

bool HeartbeatJob::sendHeartbeat() {
  try {
    client_.sendHeartbeat(heartbeatOf(
        games_.get().value().value_or(std::vector<network::HostedGame>{}),
        metrics_ != nullptr ? metrics_->value() : std::nullopt));
    return true;
  } catch (const network::MasterRefusedException& refusal) {
    tell(refusal.what());
    return refusal.status() != UNKNOWN_KEY_STATUS &&
           refusal.status() != UNREGISTERED_SERVER_STATUS;
  } catch (const network::MasterUnreachableException& silence) {
    tell(silence.what());
    return false;
  }
}

void HeartbeatJob::tell(const std::string& problem) {
  if (problem == lastProblem_) {
    return;
  }
  lastProblem_ = problem;
  logger_.warn(problem);
}

}  // namespace rtype::server
