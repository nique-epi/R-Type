#include "SimulatorLaunchOptions.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "NetworkConditions.hpp"
#include "NetworkConditionsConstants.hpp"
#include "NetworkConditionsText.hpp"
#include "NetworkException.hpp"
#include "NetworkScenario.hpp"
#include "WholeNumberText.hpp"

namespace rtype::network {

namespace {

/** @brief An option of the simulator, and how it writes its value into the
 * conditions. */
struct SimulatorOption {
  std::string_view name;
  void (*record)(NetworkConditions& conditions, std::string_view value);
};

/**
 * @returns The whole number @p value writes, from 0 to @p maximum.
 * @throws InvalidSimulatorOptionException naming @p option for any other text.
 */
int wholeNumberUpTo(std::string_view option, std::string_view value,
                    int maximum) {
  unsigned int number = 0;
  if (!readsWholeNumber(value, number) ||
      number > static_cast<unsigned int>(maximum)) {
    throw InvalidSimulatorOptionException(std::string(option),
                                          std::string(value));
  }
  return static_cast<int>(number);
}

/** @returns The lag or jitter @p value writes, in milliseconds up to
 * MAXIMUM_SIMULATED_DELAY. */
std::chrono::milliseconds delayFrom(std::string_view option,
                                    std::string_view value) {
  return std::chrono::milliseconds{wholeNumberUpTo(
      option, value, static_cast<int>(MAXIMUM_SIMULATED_DELAY.count()))};
}

/** @returns The share @p value writes, in percent up to MAXIMUM_PERCENT. */
int percentFrom(std::string_view option, std::string_view value) {
  return wholeNumberUpTo(option, value, MAXIMUM_PERCENT);
}

/**
 * @returns The conditions of the scenario named @p name.
 * @throws InvalidSimulatorOptionException when NETWORK_SCENARIOS has none of
 * that name.
 */
NetworkConditions scenarioNamed(std::string_view name) {
  const auto scenario =
      std::ranges::find(NETWORK_SCENARIOS, name, &NetworkScenario::name);
  if (scenario == NETWORK_SCENARIOS.end()) {
    throw InvalidSimulatorOptionException(std::string(NETWORK_OPTION),
                                          std::string(name));
  }
  return scenario->conditions;
}

void recordScenario(NetworkConditions& conditions, std::string_view name) {
  conditions = scenarioNamed(name);
}

void recordLag(NetworkConditions& conditions, std::string_view value) {
  conditions.lag = delayFrom(LAG_OPTION, value);
}

void recordJitter(NetworkConditions& conditions, std::string_view value) {
  conditions.jitter = delayFrom(JITTER_OPTION, value);
}

void recordLoss(NetworkConditions& conditions, std::string_view value) {
  conditions.lossPercent = percentFrom(LOSS_OPTION, value);
}

void recordDuplicates(NetworkConditions& conditions, std::string_view value) {
  conditions.duplicatePercent = percentFrom(DUPLICATES_OPTION, value);
}

void recordReorder(NetworkConditions& conditions, std::string_view value) {
  conditions.reorderPercent = percentFrom(REORDER_OPTION, value);
}

/**
 * @returns The argument after the option at @p index, which @p index then
 * points at.
 * @throws MissingSimulatorOptionValueException when the option is the last
 * argument.
 */
std::string_view valueAfter(std::span<const std::string_view> arguments,
                            std::size_t& index) {
  const std::size_t valueIndex = index + 1;
  if (valueIndex >= arguments.size()) {
    throw MissingSimulatorOptionValueException(std::string(arguments[index]));
  }
  index = valueIndex;
  return arguments[valueIndex];
}

/** @returns Every scenario of NETWORK_SCENARIOS and what it does, for the
 * usage. */
std::string scenarioList() {
  std::string text;
  for (const NetworkScenario& scenario : NETWORK_SCENARIOS) {
    if (!text.empty()) {
      text += "; ";
    }
    text += std::string(scenario.name) + " (" +
            conditionsText(scenario.conditions) + ")";
  }
  return text;
}

/** @brief Appends the lines of one option to the usage @p text: the option,
 * then its description, indented. */
void appendUsageEntry(std::string& text, std::string_view option,
                      std::string_view description) {
  text += SIMULATOR_USAGE_OPTION_INDENT;
  text += option;
  text += '\n';
  text += SIMULATOR_USAGE_DESCRIPTION_INDENT;
  text += description;
  text += '\n';
}

}  // namespace

SimulatorLaunchOptions SimulatorLaunchOptions::read(
    std::span<const std::string_view> arguments) {
  const std::array options{
      SimulatorOption{.name = NETWORK_OPTION, .record = recordScenario},
      SimulatorOption{.name = LAG_OPTION, .record = recordLag},
      SimulatorOption{.name = JITTER_OPTION, .record = recordJitter},
      SimulatorOption{.name = LOSS_OPTION, .record = recordLoss},
      SimulatorOption{.name = DUPLICATES_OPTION, .record = recordDuplicates},
      SimulatorOption{.name = REORDER_OPTION, .record = recordReorder},
  };
  NetworkConditions conditions = PERFECT_NETWORK;
  std::vector<std::string_view> otherArguments;
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const auto option =
        std::ranges::find(options, arguments[index], &SimulatorOption::name);
    if (option == options.end()) {
      otherArguments.push_back(arguments[index]);
    } else {
      option->record(conditions, valueAfter(arguments, index));
    }
  }
  return SimulatorLaunchOptions{conditions, std::move(otherArguments)};
}

const NetworkConditions& SimulatorLaunchOptions::conditions() const {
  return conditions_;
}

std::span<const std::string_view> SimulatorLaunchOptions::otherArguments()
    const {
  return otherArguments_;
}

std::string SimulatorLaunchOptions::usage() {
  const std::string longestDelay =
      std::to_string(MAXIMUM_SIMULATED_DELAY.count());
  const std::string largestShare = std::to_string(MAXIMUM_PERCENT);
  std::string text{SIMULATOR_USAGE_TITLE};
  text += '\n';
  appendUsageEntry(
      text, std::string(NETWORK_OPTION) + " <name>",
      "A simulated network by name, which sets the five values below: " +
          scenarioList() +
          ". An option given after it changes one of its values. Default: "
          "none, the network is not simulated.");
  appendUsageEntry(text, std::string(LAG_OPTION) + " <ms>",
                   "Milliseconds the ping gains, from 0 to " + longestDelay +
                       ": each datagram sent or received waits half of them. "
                       "Default: 0.");
  appendUsageEntry(
      text, std::string(JITTER_OPTION) + " <ms>",
      "Milliseconds the ping may vary by, from 0 to " + longestDelay +
          ": each datagram waits up to half of them more, drawn at random, "
          "and never arrives before the one sent before it. Default: 0.");
  appendUsageEntry(text, std::string(LOSS_OPTION) + " <percent>",
                   "Percent of the datagrams sent or received that never "
                   "arrive, from 0 to " +
                       largestShare + ". Default: 0.");
  appendUsageEntry(text, std::string(DUPLICATES_OPTION) + " <percent>",
                   "Percent of the datagrams that arrive twice, from 0 to " +
                       largestShare + ". Default: 0.");
  appendUsageEntry(
      text, std::string(REORDER_OPTION) + " <percent>",
      "Percent of the datagrams held " + std::to_string(REORDER_HOLD.count()) +
          " ms more, so the ones sent just after them arrive first, from 0 "
          "to " +
          largestShare + ". Default: 0.");
  return text;
}

SimulatorLaunchOptions::SimulatorLaunchOptions(
    NetworkConditions conditions, std::vector<std::string_view> otherArguments)
    : conditions_(conditions), otherArguments_(std::move(otherArguments)) {}

}  // namespace rtype::network
