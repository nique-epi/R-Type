#include "BenchLaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include "BenchLaunch.hpp"
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "ScriptBudgetBenchConstants.hpp"
#include "WholeNumberOption.hpp"

namespace rtype::tools {

namespace {

/** @returns The option @p name, whose value is a whole number written into
 * @p field. */
engine::LaunchOption countOption(std::string_view name, std::size_t& field) {
  return engine::LaunchOption{
      .name = name,
      .takesValue = true,
      .record = [name, &field](std::string_view text) {
        field = server::wholeNumberFromText<std::uint32_t>(name, text);
      }};
}

/** @returns TICKS_OPTION, whose value is written into @p field: a run of no
 * tick has no figure to report. */
engine::LaunchOption ticksOption(std::size_t& field) {
  return engine::LaunchOption{
      .name = TICKS_OPTION,
      .takesValue = true,
      .record = [&field](std::string_view text) {
        const auto ticks =
            server::wholeNumberFromText<std::uint32_t>(TICKS_OPTION, text);
        if (ticks == 0) {
          throw engine::InvalidLaunchOptionValueException(
              std::string(TICKS_OPTION), std::string(text));
        }
        field = ticks;
      }};
}

/** @returns The option @p name, with no value, which sets @p flag. */
engine::LaunchOption flagOption(std::string_view name, bool& flag) {
  return engine::LaunchOption{
      .name = name,
      .takesValue = false,
      .record = [&flag]([[maybe_unused]] std::string_view value) {
        flag = true;
      }};
}

}  // namespace

BenchLaunch readBenchLaunch(std::span<const std::string_view> arguments) {
  BenchLaunch launch;
  const std::array options{
      countOption(SCRIPTED_BODIES_OPTION, launch.population.scriptedBodies),
      countOption(BULLETS_OPTION, launch.population.bullets),
      ticksOption(launch.ticks),
      flagOption(WAITING_OPTION, launch.population.waiting),
      flagOption(engine::HELP_OPTION, launch.helpRequested)};
  engine::readLaunchArguments(arguments, options);
  return launch;
}

std::string benchUsage() {
  std::string text = engine::usageHeader(SCRIPT_BENCH_PROGRAM_NAME);
  text += engine::usageEntry(
      std::string(SCRIPTED_BODIES_OPTION) + " <count>",
      "Bodies steered by the script, kept alive at every tick. Default: " +
          std::to_string(DEFAULT_SCRIPTED_BODIES) + ".");
  text += engine::usageEntry(std::string(BULLETS_OPTION) + " <count>",
                             "Bullets kept alive at every tick. Default: " +
                                 std::to_string(DEFAULT_BULLETS) + ".");
  text += engine::usageEntry(std::string(TICKS_OPTION) + " <count>",
                             "Ticks to measure, from 1. Default: " +
                                 std::to_string(DEFAULT_BENCH_TICKS) + ".");
  text += engine::usageEntry(
      std::string(WAITING_OPTION),
      "Run a script that waits and is resumed at every tick.");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and measure nothing.");
  return text;
}

}  // namespace rtype::tools
