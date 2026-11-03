#include "MetricsLines.hpp"
#include <sstream>
#include <string>
#include <string_view>
#include "DurationConversion.hpp"
#include "GamesCensus.hpp"
#include "MetricsReport.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string ticksLineOf(const MetricsReport& report) {
  const TickDurationSummary& ticks = report.ticks;
  const GamesCensus& games = report.games;
  std::ostringstream line;
  line << ticks.count << " ticks lasted "
       << engine::inMilliseconds(ticks.average) << " ms on average, "
       << engine::inMilliseconds(ticks.median) << " ms at the median, "
       << engine::inMilliseconds(ticks.ninetyFifthPercentile)
       << " ms at the 95th percentile and "
       << engine::inMilliseconds(ticks.worst) << " ms at most, for a budget of "
       << engine::inMilliseconds(engine::SIMULATION_TICK_DURATION)
       << " ms; games in progress: " << games.inProgress << " of "
       << games.hosted << "; players connected: " << games.players
       << "; entities: " << games.entities;
  return line.str();
}

std::string systemsLineOf(const MetricsReport& report) {
  std::ostringstream line;
  line << "average time per tick:";
  std::string_view separator = " ";
  for (const engine::SystemTime& time : report.systemTimes) {
    line << separator << time.name << ' '
         << engine::inMilliseconds(
                engine::averageOf(time.total, report.ticks.count))
         << " ms";
    separator = ", ";
  }
  return line.str();
}

}  // namespace rtype::server
