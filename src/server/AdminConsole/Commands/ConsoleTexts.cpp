#include "ConsoleTexts.hpp"
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <ios>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include "AdminConsoleConstants.hpp"
#include "GameLifecycle.hpp"
#include "MeasureMilliseconds.hpp"
#include "NameList.hpp"
#include "ServerMeasures.hpp"
#include "SupervisedGame.hpp"
#include "TrafficLine.hpp"
#include "TrafficTotals.hpp"

namespace rtype::server {

namespace {

/** @returns ", <label> <value><unit>" when @p value was taken, with
 * MEASURE_DECIMALS decimals; nothing otherwise. */
std::string measureText(std::string_view label,
                        const std::optional<double>& value,
                        std::string_view unit) {
  if (!value.has_value()) {
    return {};
  }
  std::ostringstream text;
  text << ", " << label << ' ' << std::fixed
       << std::setprecision(MEASURE_DECIMALS) << *value << unit;
  return text.str();
}

std::string gameLineOf(const SupervisedGame& game) {
  return "place " + std::to_string(game.slot) + ": game " +
         std::to_string(game.number) + ", " +
         std::string{lifecycleText(game.lifecycle)} + ", " +
         std::to_string(game.nicknames.size()) + " pilots, port " +
         std::to_string(game.port) +
         measureText("tick p95",
                     millisecondsOf(
                         game.measures.tickNinetyFifthPercentileMicroseconds),
                     " ms") +
         measureText("ping",
                     millisecondsOf(game.measures.averageRoundTripMicroseconds),
                     " ms");
}

std::string playerLineOf(const SupervisedGame& game) {
  return "place " + std::to_string(game.slot) + ": " +
         (game.nicknames.empty() ? std::string{NO_PILOT_TEXT}
                                 : engine::joinedNames(game.nicknames));
}

/** @returns The line @p lineOf writes for each game of @p games, or "no game
 * runs" when there is none. */
std::vector<std::string> linesOf(std::span<const SupervisedGame> games,
                                 std::string (*lineOf)(const SupervisedGame&)) {
  if (games.empty()) {
    return {std::string{NO_GAME_TEXT}};
  }
  std::vector<std::string> lines;
  lines.reserve(games.size());
  for (const SupervisedGame& game : games) {
    lines.push_back(lineOf(game));
  }
  return lines;
}

}  // namespace

std::vector<std::string> gameLinesOf(std::span<const SupervisedGame> games) {
  return linesOf(games, gameLineOf);
}

std::vector<std::string> playerLinesOf(std::span<const SupervisedGame> games) {
  return linesOf(games, playerLineOf);
}

std::string closingLineOf(const SupervisedGame& game) {
  return "game " + std::to_string(game.number) + " in place " +
         std::to_string(game.slot) +
         " told to stop: it leaves the list once its process ended";
}

std::vector<std::string> networkLinesOf(const network::TrafficTotals& traffic,
                                        std::chrono::nanoseconds elapsed,
                                        std::size_t rejected) {
  return {
      "supervisor socket, " +
          network::trafficLineOf(network::TrafficTotals{}, traffic, elapsed),
      "rejected since the start: " + std::to_string(rejected) + " datagrams"};
}

std::string measuresLineOf(
    const std::optional<network::ServerMeasures>& measures) {
  if (!measures.has_value()) {
    return std::string{NO_MEASURE_TEXT};
  }
  const std::string taken =
      measureText("tick p50", measures->tickP50Milliseconds, " ms") +
      measureText("tick p95", measures->tickP95Milliseconds, " ms") +
      measureText("received", measures->receivedBytesPerSecond, " B/s") +
      measureText("sent", measures->sentBytesPerSecond, " B/s") +
      measureText("loss", measures->lossPercent, " %") +
      measureText("ping", measures->averagePingMilliseconds, " ms");
  return taken.empty() ? std::string{NO_MEASURE_TEXT}
                       : "games: measured" + taken;
}

}  // namespace rtype::server
