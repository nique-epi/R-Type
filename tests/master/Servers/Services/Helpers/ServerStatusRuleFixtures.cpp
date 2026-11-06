#include "ServerStatusRuleFixtures.hpp"
#include <optional>
#include "ServerMeasures.hpp"
#include "ServerStatusRuleTestConstants.hpp"
#include "StatusEvidence.hpp"

rtype::network::ServerMeasures measuresWith(std::optional<double> tickP95,
                                            std::optional<double> loss) {
  return rtype::network::ServerMeasures{
      .tickP50Milliseconds = std::nullopt,
      .tickP95Milliseconds = tickP95,
      .receivedBytesPerSecond = std::nullopt,
      .sentBytesPerSecond = std::nullopt,
      .lossPercent = loss,
      .averagePingMilliseconds = std::nullopt};
}

rtype::master::StatusEvidence heardWith(
    const rtype::network::ServerMeasures& measures) {
  return rtype::master::StatusEvidence{.registeredAt = HEARD_AT,
                                       .lastHeartbeatAt = HEARD_AT,
                                       .measures = measures,
                                       .maintenance = false,
                                       .lastProbeAnswerAt = HEARD_AT};
}
