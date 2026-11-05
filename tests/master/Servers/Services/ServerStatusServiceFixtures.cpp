#include "ServerStatusServiceFixtures.hpp"
#include <optional>
#include "ServerMeasures.hpp"
#include "ServerStatusServiceTestConstants.hpp"

rtype::network::ServerMeasures parisMeasuresWith(double tickP95) {
  return rtype::network::ServerMeasures{
      .tickP50Milliseconds = std::nullopt,
      .tickP95Milliseconds = tickP95,
      .receivedBytesPerSecond = std::nullopt,
      .sentBytesPerSecond = std::nullopt,
      .lossPercent = QUIET_LOSS_OF_PARIS,
      .averagePingMilliseconds = std::nullopt};
}
