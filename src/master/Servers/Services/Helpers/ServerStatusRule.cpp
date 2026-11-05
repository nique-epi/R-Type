#include "ServerStatusRule.hpp"
#include <chrono>
#include <optional>
#include "ServerStatus.hpp"
#include "ServerStatusRuleConstants.hpp"
#include "StatusEvidence.hpp"

namespace rtype::master {

namespace {

/** @returns Whether @p measure was taken and reaches @p bound. */
bool reaches(std::optional<double> measure, double bound) {
  return measure.has_value() && *measure >= bound;
}

}  // namespace

network::ServerStatus statusOf(const StatusEvidence& evidence,
                               std::chrono::sys_seconds now) {
  if (evidence.maintenance) {
    return network::ServerStatus::Maintenance;
  }
  if (now - evidence.lastHeardAt >= OFFLINE_SILENCE) {
    return network::ServerStatus::Offline;
  }
  if (reaches(evidence.measures.tickP95Milliseconds,
              DEGRADED_TICK_P95_MILLISECONDS) ||
      reaches(evidence.measures.lossPercent, DEGRADED_LOSS_PERCENT)) {
    return network::ServerStatus::Degraded;
  }
  return network::ServerStatus::Online;
}

}  // namespace rtype::master
