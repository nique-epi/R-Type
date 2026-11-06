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

/** @returns Whether a probe got a valid answer at @p answeredAt, less than
 * PROBE_ANSWER_SILENCE before @p now. */
bool answeredRecently(std::optional<std::chrono::sys_seconds> answeredAt,
                      std::chrono::sys_seconds now) {
  return answeredAt.has_value() && now - *answeredAt < PROBE_ANSWER_SILENCE;
}

}  // namespace

network::ServerStatus statusOf(const StatusEvidence& evidence,
                               std::chrono::sys_seconds now) {
  if (evidence.maintenance) {
    return network::ServerStatus::Maintenance;
  }
  const std::chrono::sys_seconds lastHeardAt =
      evidence.lastHeartbeatAt.value_or(evidence.registeredAt);
  if (now - lastHeardAt >= OFFLINE_SILENCE) {
    return network::ServerStatus::Offline;
  }
  if (!answeredRecently(evidence.lastProbeAnswerAt, now)) {
    return network::ServerStatus::Unreachable;
  }
  if (reaches(evidence.measures.tickP95Milliseconds,
              DEGRADED_TICK_P95_MILLISECONDS) ||
      reaches(evidence.measures.lossPercent, DEGRADED_LOSS_PERCENT)) {
    return network::ServerStatus::Degraded;
  }
  return network::ServerStatus::Online;
}

}  // namespace rtype::master
