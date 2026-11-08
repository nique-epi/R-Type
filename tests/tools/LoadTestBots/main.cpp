#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <span>
#include <string_view>
#include <thread>
#include <vector>
#include "BotConstants.hpp"
#include "BotLaunch.hpp"
#include "BotLaunchConstants.hpp"
#include "BotLaunchOptions.hpp"
#include "BotReport.hpp"
#include "BotSquad.hpp"
#include "BotTraffic.hpp"
#include "ConnectionPhase.hpp"
#include "FineTimerResolution.hpp"
#include "LaunchArguments.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"

namespace {

using rtype::client::ConnectionPhase;
using rtype::tools::BotLaunch;
using rtype::tools::BotReport;
using rtype::tools::BotSquad;

/** @returns What @p phase reads as in the report. */
std::string_view phaseText(ConnectionPhase phase) {
  switch (phase) {
    case ConnectionPhase::Idle:
      return "idle";
    case ConnectionPhase::Connecting:
      return "still connecting";
    case ConnectionPhase::Accepted:
      return "in session";
    case ConnectionPhase::Refused:
      return "refused";
    case ConnectionPhase::Unreachable:
      return "unreachable";
    case ConnectionPhase::Lost:
      return "lost";
    case ConnectionPhase::Closed:
      return "closed by the server";
    case ConnectionPhase::Expelled:
      return "expelled by the host";
    case ConnectionPhase::LoadingContent:
      return "loading the content of the server";
    case ConnectionPhase::ContentDownloadFailed:
      return "left: content pack not downloaded";
    case ConnectionPhase::ContentRefused:
      return "left: content pack refused";
  }
  return "unknown";
}

/** @returns How many of @p reports are still in session. */
std::size_t inSessionCount(std::span<const BotReport> reports) {
  return static_cast<std::size_t>(std::ranges::count(
      reports, ConnectionPhase::Accepted, &BotReport::phase));
}

/** @returns The time @p report spent in a game, in seconds. */
double secondsInGame(const BotReport& report) {
  return std::chrono::duration<double>(report.timeInGame).count();
}

/** @returns @p count per second of the game time of @p report; 0 before any
 * game. */
double perSecondOfGame(std::uint64_t count, const BotReport& report) {
  const double seconds = secondsInGame(report);
  return seconds > 0.0 ? static_cast<double>(count) / seconds : 0.0;
}

/** @returns The entities a world state of @p report listed on average; 0
 * before any state. */
double entitiesPerState(const BotReport& report) {
  return report.worldStates > 0 ? static_cast<double>(report.entitiesListed) /
                                      static_cast<double>(report.worldStates)
                                : 0.0;
}

/** @brief Prints the line of bot @p number: its phase, its games, its world
 * states and inputs per second, the entities of a state, then what its socket
 * received, decoded and as the network carried it, and sent per second of
 * game. The client compresses nothing, so what it sent is read on the wire. */
void printBotLine(std::size_t number, const BotReport& report) {
  const rtype::tools::BotTraffic& traffic = report.trafficInGame;
  std::cout << "bot " << number << ": " << phaseText(report.phase) << ", "
            << report.gamesEnded << " games ended, " << report.worldStates
            << " world states and " << report.inputsInGame << " inputs in "
            << secondsInGame(report)
            << " s of game: " << perSecondOfGame(report.worldStates, report)
            << " and " << perSecondOfGame(report.inputsInGame, report)
            << " per second, " << entitiesPerState(report)
            << " entities per state; received "
            << perSecondOfGame(traffic.plain.received.bytes, report)
            << " B/s decoded and "
            << perSecondOfGame(traffic.wire.received.bytes, report)
            << " B/s on the network in "
            << perSecondOfGame(traffic.wire.received.count, report)
            << " datagrams/s, sent "
            << perSecondOfGame(traffic.wire.sent.bytes, report) << " B/s in "
            << perSecondOfGame(traffic.wire.sent.count, report)
            << " datagrams/s\n";
}

/** @brief Prints what @p launch asked, one line per bot of @p reports, then
 * how many are still in session. */
void printReport(const BotLaunch& launch, std::span<const BotReport> reports) {
  std::cout << rtype::tools::BOTS_PROGRAM_NAME << ": " << reports.size()
            << " bots for " << launch.duration.count() << " s on "
            << launch.server.address << ':' << launch.server.port << ", seed "
            << launch.seed << '\n'
            << std::fixed << std::setprecision(1);
  std::size_t number = 1;
  for (const BotReport& report : reports) {
    printBotLine(number++, report);
  }
  std::cout << "in session at the end: " << inSessionCount(reports) << " of "
            << reports.size() << " bots\n";
}

/** @brief Says on the error output that the system refused @p fineTimers, so
 * the frames may start late. */
void warnWhenRefused(const rtype::server::FineTimerResolution& fineTimers) {
  if (!fineTimers.isGranted()) {
    std::cerr << rtype::tools::BOTS_PROGRAM_NAME
              << ": the system refused a timer resolution of "
              << rtype::server::FINE_TIMER_RESOLUTION_MILLISECONDS
              << " ms, so frames may start late\n";
  }
}

/**
 * @brief Moves @p squad every BOT_FRAME_DURATION for @p duration, by the time
 * each frame really lasted.
 *
 * Each frame has a deadline one BOT_FRAME_DURATION after the previous one, so
 * a late wake-up never delays the frames after it: waking BOT_FRAME_DURATION
 * after the last update instead ran about 44 frames per second on macOS, and
 * each bot then sent fewer inputs than a client. A fine timer resolution is
 * held meanwhile, so Windows wakes the loop close to each deadline.
 */
void playFor(BotSquad& squad, std::chrono::seconds duration) {
  const rtype::server::FineTimerResolution fineTimers;
  warnWhenRefused(fineTimers);
  const auto start = std::chrono::steady_clock::now();
  auto previous = start;
  auto deadline = start;
  while (previous - start < duration) {
    deadline += rtype::tools::BOT_FRAME_DURATION;
    std::this_thread::sleep_until(deadline);
    const auto now = std::chrono::steady_clock::now();
    squad.update(
        std::chrono::duration_cast<rtype::engine::Duration>(now - previous));
    previous = now;
  }
}

/**
 * @returns The exit code of the run @p launch asks: 0 when every bot is still
 * in session at the end, 1 otherwise. The report is printed before the bots
 * leave, so a bot that fails to leave cannot hide it. An error stops the run
 * and is printed: no exception leaves it.
 */
int runBots(const BotLaunch& launch) {
  try {
    BotSquad squad{launch};
    playFor(squad, launch.duration);
    const std::vector<BotReport> reports = squad.report();
    printReport(launch, reports);
    squad.leave();
    return inSessionCount(reports) == reports.size() ? 0 : 1;
  } catch (const std::exception& error) {
    std::cerr << rtype::tools::BOTS_PROGRAM_NAME << " stopped: " << error.what()
              << '\n';
    return 1;
  }
}

}  // namespace

int main(int argumentCount, char** arguments) {
  try {
    const BotLaunch launch = rtype::tools::readBotLaunch(
        rtype::engine::processArguments(argumentCount, arguments));
    if (launch.helpRequested) {
      std::cout << rtype::tools::botUsage();
      return 0;
    }
    return runBots(launch);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(rtype::tools::BOTS_PROGRAM_NAME,
                                                error.what());
    return 1;
  }
}
