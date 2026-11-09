#include <chrono>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <string_view>
#include <vector>
#include "BenchContentFolders.hpp"
#include "BenchLaunch.hpp"
#include "BenchLaunchOptions.hpp"
#include "BenchScene.hpp"
#include "DurationSpread.hpp"
#include "LaunchArguments.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "TickFigures.hpp"
#include "TickSample.hpp"
#include "TimeConstants.hpp"

namespace {

using rtype::engine::Duration;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::tools::BenchLaunch;
using rtype::tools::BenchScene;
using rtype::tools::DurationSpread;
using rtype::tools::TickFigures;
using rtype::tools::TickSample;

constexpr double PERCENT = 100.0;

#ifdef NDEBUG
constexpr bool OPTIMIZED_BUILD = true;
#else
constexpr bool OPTIMIZED_BUILD = false;
#endif

/** @returns @p duration in milliseconds. */
double milliseconds(Duration duration) {
  return std::chrono::duration<double, std::milli>(duration).count();
}

/** @returns What each tick of @p launch took, computed one after the other
 * with no wait, so two runs measure the same work.
 * @param[out] scene The world they ran in. */
std::vector<TickSample> measure(const BenchLaunch& launch, BenchScene& scene) {
  std::vector<TickSample> samples;
  samples.reserve(launch.ticks);
  for (std::size_t tick = 0; tick < launch.ticks; ++tick) {
    samples.push_back(scene.runTick());
  }
  return samples;
}

/** @brief Prints the line @p label of @p spread. */
void printSpread(std::string_view label, const DurationSpread& spread) {
  std::cout << label << ": median " << milliseconds(spread.median)
            << " ms, 99th percentile " << milliseconds(spread.high)
            << " ms, worst " << milliseconds(spread.worst) << " ms\n";
}

/** @brief Prints what @p launch asked, then @p figures. */
void printReport(const BenchLaunch& launch, const TickFigures& figures) {
  std::cout << std::fixed << std::setprecision(3)
            << rtype::tools::SCRIPT_BENCH_PROGRAM_NAME << ": "
            << launch.population.scriptedBodies << " scripted bodies, "
            << launch.population.bullets << " bullets, " << launch.ticks
            << " ticks of " << milliseconds(SIMULATION_TICK_DURATION)
            << " ms\n";
  printSpread("tick", figures.tick);
  printSpread("scripts", figures.scripts);
  std::cout << "share of the scripts: " << figures.scriptShare * PERCENT
            << " %\nmedian tick: "
            << milliseconds(figures.tick.median) /
                   milliseconds(SIMULATION_TICK_DURATION) * PERCENT
            << " % of the budget\nticks over the budget: "
            << figures.ticksOverBudget << " of " << launch.ticks << '\n';
}

/** @brief Measures what @p launch asks and prints the report.
 * @returns The exit code of the program. */
int runBench(const BenchLaunch& launch) {
  if (!OPTIMIZED_BUILD) {
    std::cerr << rtype::tools::SCRIPT_BENCH_PROGRAM_NAME
              << ": this build is not optimized, its figures say nothing of "
                 "a server\n";
  }
  const std::vector<std::filesystem::path> folders =
      rtype::tools::benchContentFolders();
  BenchScene scene{launch.population, folders};
  const std::vector<TickSample> samples = measure(launch, scene);
  if (!scene.scriptFailure().empty()) {
    std::cerr << rtype::tools::SCRIPT_BENCH_PROGRAM_NAME << ": "
              << scene.scriptFailure() << '\n';
    return 1;
  }
  std::cout << "memory of the scripts at the end: " << scene.scriptMemory()
            << " bytes\n";
  printReport(launch,
              rtype::tools::figuresOf(samples, SIMULATION_TICK_DURATION));
  return 0;
}

}  // namespace

int main(int argumentCount, char** arguments) {
  try {
    const BenchLaunch launch = rtype::tools::readBenchLaunch(
        rtype::engine::processArguments(argumentCount, arguments));
    if (launch.helpRequested) {
      std::cout << rtype::tools::benchUsage();
      return 0;
    }
    return runBench(launch);
  } catch (const std::exception& error) {
    std::cerr << rtype::tools::SCRIPT_BENCH_PROGRAM_NAME << ": " << error.what()
              << '\n';
    return 1;
  }
}
