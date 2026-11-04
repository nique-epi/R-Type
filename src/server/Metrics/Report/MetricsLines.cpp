#include "MetricsLines.hpp"
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <string_view>
#include "DurationConversion.hpp"
#include "GamesCensus.hpp"
#include "MetricsReport.hpp"
#include "PlayerRoundTripMeasure.hpp"
#include "SystemTime.hpp"
#include "TickDurationSummary.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

constexpr unsigned char FIRST_PRINTABLE_BYTE = 0x20;
constexpr unsigned char LAST_PRINTABLE_BYTE = 0x7E;
constexpr int HEXADECIMAL_DIGITS_PER_BYTE = 2;

/**
 * @returns @p nickname between double quotes, with the quote and the backslash
 * escaped and every byte outside the printable ASCII written as \xNN, so that
 * a nickname chosen by a client can neither end its line nor pass for another
 * field of it.
 */
std::string quotedNickname(std::string_view nickname) {
  std::ostringstream quoted;
  quoted << '"';
  for (const char character : nickname) {
    const auto byte = static_cast<unsigned char>(character);
    if (byte < FIRST_PRINTABLE_BYTE || byte > LAST_PRINTABLE_BYTE) {
      quoted << "\\x" << std::hex << std::setw(HEXADECIMAL_DIGITS_PER_BYTE)
             << std::setfill('0') << static_cast<unsigned int>(byte)
             << std::dec;
    } else if (character == '"' || character == '\\') {
      quoted << '\\' << character;
    } else {
      quoted << character;
    }
  }
  quoted << '"';
  return quoted.str();
}

}  // namespace

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

std::string roundTripsLineOf(const MetricsReport& report) {
  std::ostringstream line;
  line << "round trips:";
  if (report.roundTrips.empty()) {
    line << " no player measured";
    return line.str();
  }
  std::string_view separator = " ";
  for (const PlayerRoundTripMeasure& measure : report.roundTrips) {
    line << separator << "game " << measure.gameNumber << " player "
         << static_cast<unsigned int>(measure.playerNumber) << ' '
         << quotedNickname(measure.nickname) << ' '
         << engine::inMilliseconds(measure.figures.roundTrip) << " ms, jitter "
         << engine::inMilliseconds(measure.figures.jitter) << " ms";
    separator = "; ";
  }
  return line.str();
}

}  // namespace rtype::server
