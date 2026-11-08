#include "HeartbeatJob.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <random>
#include <stop_token>
#include <string>
#include <utility>
#include <vector>
#include "HeartbeatJobParts.hpp"
#include "HostedGame.hpp"
#include "InterruptibleSleep.hpp"
#include "MasterMessages.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"
#include "ServerHeartbeat.hpp"
#include "ServerMasterConstants.hpp"

namespace rtype::server {

namespace {

/** @returns What is left of @p gap since @p sentAt, zero once it passed. */
std::chrono::milliseconds untilTheGapEnds(
    std::chrono::steady_clock::time_point sentAt,
    std::chrono::milliseconds gap) {
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - sentAt);
  return std::max(gap - elapsed, std::chrono::milliseconds{0});
}

}  // namespace

HeartbeatJob::HeartbeatJob(HeartbeatJobParts parts)
    : client_(std::move(parts.access)),
      courier_(std::move(parts.resultsFile), client_, problems_),
      identity_(std::move(parts.identity)),
      games_(parts.games),
      difficulties_(std::move(parts.difficulties)),
      probeTokens_(parts.probeTokens),
      measures_(parts.measures),
      times_(parts.times),
      backoff_(parts.times),
      drawer_(std::random_device{}()),
      logger_(std::string{SERVER_LOGGER_NAME}),
      masterLink_([this](const std::stop_token& stop) { run(stop); }) {}

void HeartbeatJob::run(const std::stop_token& stop) {
  bool registered = false;
  std::uint64_t seen = 0;
  while (!stop.stop_requested()) {
    if (!registered) {
      registered = registerServer();
      if (!registered) {
        concurrency::sleepUnlessStopped(stop, nextRetryWait());
        continue;
      }
    }
    seen = games_.revision();
    registered = sendHeartbeat();
    const auto sentAt = std::chrono::steady_clock::now();
    if (registered) {
      courier_.sendWaitingResults(stop);
    }
    if (games_.waitForNewer(seen, untilTheGapEnds(sentAt, times_.interval),
                            stop) > seen) {
      concurrency::sleepUnlessStopped(
          stop, untilTheGapEnds(sentAt, times_.noticeGap));
    }
  }
}

bool HeartbeatJob::registerServer() {
  try {
    client_.registerServer(registrationOf(identity_));
    logger_.info("registered with the master as ", identity_.name);
    problems_.forget();
    backoff_.reset();
    return true;
  } catch (const network::MasterRefusedException& refusal) {
    problems_.tell(refusal.what());
  } catch (const network::MasterUnreachableException& silence) {
    problems_.tell(silence.what());
  }
  return false;
}

bool HeartbeatJob::sendHeartbeat() {
  const std::uint64_t tokenRevision = probeTokens_.revision();
  try {
    network::ServerHeartbeat heartbeat =
        heartbeatOf(games_.value().value_or(std::vector<network::HostedGame>{}),
                    measures_ != nullptr ? measures_->value() : std::nullopt,
                    unsentProbeToken(tokenRevision));
    heartbeat.difficulties = difficulties_;
    client_.sendHeartbeat(heartbeat);
    sentTokenRevision_ = tokenRevision;
    return true;
  } catch (const network::MasterRefusedException& refusal) {
    problems_.tell(refusal.what());
    return refusal.status() != UNKNOWN_KEY_STATUS &&
           refusal.status() != UNREGISTERED_SERVER_STATUS;
  } catch (const network::MasterUnreachableException& silence) {
    problems_.tell(silence.what());
    return false;
  }
}

std::optional<std::uint64_t> HeartbeatJob::unsentProbeToken(
    std::uint64_t revision) const {
  if (revision <= sentTokenRevision_) {
    return std::nullopt;
  }
  return probeTokens_.value();
}

std::chrono::milliseconds HeartbeatJob::nextRetryWait() {
  std::uniform_real_distribution<double> share{0.0, 1.0};
  return backoff_.nextWait(share(drawer_));
}

}  // namespace rtype::server
