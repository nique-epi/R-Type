#include "LatestMetricsFixtures.hpp"
#include <cstddef>
#include <vector>
#include "GamesCensus.hpp"
#include "LatestMetricsTestConstants.hpp"
#include "MetricsReport.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"

namespace rtype::server::testing {

MetricsReport reportCounting(std::size_t count) {
  return MetricsReport{
      .ticks = TickDurationSummary{},
      .games = GamesCensus{.players = count, .entities = count},
      .systemTimes = std::vector<engine::SystemTime>(count % MOST_SYSTEM_TIMES),
      .traffic = {},
      .roundTrips = {}};
}

}  // namespace rtype::server::testing
