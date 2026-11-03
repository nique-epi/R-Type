#include "LaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "WholeNumberText.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The whole number @p text is made of, digits only.
 * @throws engine::InvalidLaunchOptionValueException naming @p option when it
 * is anything else, or does not fit in a Number.
 */
template <typename Number>
Number wholeNumberFromText(std::string_view option, std::string_view text) {
  const std::optional<Number> number = network::wholeNumberIn<Number>(text);
  if (!number.has_value()) {
    throw engine::InvalidLaunchOptionValueException(std::string(option),
                                                    std::string(text));
  }
  return *number;
}

/**
 * @returns The seed @p text writes, from 0 to 4294967295.
 * @throws engine::InvalidLaunchOptionValueException when it is not one.
 */
std::uint32_t seedFromText(std::string_view text) {
  return wholeNumberFromText<std::uint32_t>(SEED_OPTION, text);
}

/**
 * @returns The state rate @p text writes, from LOWEST_WORLD_STATES_PER_SECOND
 * to HIGHEST_WORLD_STATES_PER_SECOND.
 * @throws engine::InvalidLaunchOptionValueException when it is not one.
 */
std::size_t stateRateFromText(std::string_view text) {
  const auto rate = wholeNumberFromText<std::size_t>(STATE_RATE_OPTION, text);
  if (rate < LOWEST_WORLD_STATES_PER_SECOND ||
      rate > HIGHEST_WORLD_STATES_PER_SECOND) {
    throw engine::InvalidLaunchOptionValueException(
        std::string(STATE_RATE_OPTION), std::string(text));
  }
  return rate;
}

/**
 * @returns The input delay @p text writes, from LOWEST_INPUT_DELAY_TICKS to
 * HIGHEST_INPUT_DELAY_TICKS.
 * @throws engine::InvalidLaunchOptionValueException when it is not one.
 */
std::size_t inputDelayFromText(std::string_view text) {
  const auto delay = wholeNumberFromText<std::size_t>(INPUT_DELAY_OPTION, text);
  if (delay < LOWEST_INPUT_DELAY_TICKS || delay > HIGHEST_INPUT_DELAY_TICKS) {
    throw engine::InvalidLaunchOptionValueException(
        std::string(INPUT_DELAY_OPTION), std::string(text));
  }
  return delay;
}

/** @returns `-p <port>`, which writes a checked port into @p port. */
engine::LaunchOption portOption(std::uint16_t& port) {
  return engine::LaunchOption{.name = engine::PORT_OPTION,
                              .takesValue = true,
                              .record = [&port](std::string_view text) {
                                port = network::portFromText(text);
                              }};
}

/** @returns `--seed <seed>`, which writes a checked seed into @p seed. */
engine::LaunchOption seedOption(std::optional<std::uint32_t>& seed) {
  return engine::LaunchOption{
      .name = SEED_OPTION,
      .takesValue = true,
      .record = [&seed](std::string_view text) { seed = seedFromText(text); }};
}

/**
 * @returns `--state-rate <states>`, which writes a checked rate into
 * @p stateRate.
 */
engine::LaunchOption stateRateOption(std::size_t& stateRate) {
  return engine::LaunchOption{.name = STATE_RATE_OPTION,
                              .takesValue = true,
                              .record = [&stateRate](std::string_view text) {
                                stateRate = stateRateFromText(text);
                              }};
}

/**
 * @returns `--input-delay <ticks>`, which writes a checked delay into
 * @p inputDelay.
 */
engine::LaunchOption inputDelayOption(std::size_t& inputDelay) {
  return engine::LaunchOption{.name = INPUT_DELAY_OPTION,
                              .takesValue = true,
                              .record = [&inputDelay](std::string_view text) {
                                inputDelay = inputDelayFromText(text);
                              }};
}

/** @returns `--help`, which sets @p helpRequested. */
engine::LaunchOption helpOption(bool& helpRequested) {
  return engine::LaunchOption{
      .name = engine::HELP_OPTION,
      .takesValue = false,
      .record = [&helpRequested]([[maybe_unused]] std::string_view value) {
        helpRequested = true;
      }};
}

/** @returns The lines of the usage that describe the options of the server
 * itself, `--help` included. */
std::string serverOptionEntries() {
  std::string text = engine::usageEntry(
      std::string(engine::PORT_OPTION) + " <port>",
      "UDP port to listen on, from " + std::to_string(network::LOWEST_PORT) +
          " to " + std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_SERVER_PORT) + ".");
  text += engine::usageEntry(
      std::string(SEED_OPTION) + " <seed>",
      "Whole number the random draws of the games start from, from 0 to "
      "4294967295. Default: drawn at random, and logged when a game starts so "
      "it can be given here to replay it.");
  text += engine::usageEntry(
      std::string(STATE_RATE_OPTION) + " <states>",
      "World states sent to each player per second, from " +
          std::to_string(LOWEST_WORLD_STATES_PER_SECOND) + " to " +
          std::to_string(HIGHEST_WORLD_STATES_PER_SECOND) +
          ". More states move the other ships more smoothly and use more "
          "bandwidth. Default: " +
          std::to_string(DEFAULT_WORLD_STATES_PER_SECOND) + ".");
  text += engine::usageEntry(
      std::string(INPUT_DELAY_OPTION) + " <ticks>",
      "Ticks every player's input is held before it is applied, from " +
          std::to_string(LOWEST_INPUT_DELAY_TICKS) + " to " +
          std::to_string(HIGHEST_INPUT_DELAY_TICKS) +
          ". The same delay holds for every player, so it adds the same lag to "
          "all of them. Default: " +
          std::to_string(DEFAULT_INPUT_DELAY_TICKS) + ".");
  text +=
      engine::usageEntry(std::string(network::TRAFFIC_CSV_OPTION) + " <file>",
                         network::TRAFFIC_CSV_USAGE);
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  return text;
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  const network::SimulatorLaunchOptions simulatorOptions =
      network::SimulatorLaunchOptions::read(arguments);
  LaunchOptions options{network::DEFAULT_SERVER_PORT, false};
  options.networkConditions_ = simulatorOptions.conditions();
  const std::array launchOptions{
      portOption(options.port_),
      seedOption(options.seed_),
      stateRateOption(options.stateRate_),
      inputDelayOption(options.inputDelay_),
      engine::pathOption(network::TRAFFIC_CSV_OPTION, options.trafficCsvFile_),
      helpOption(options.helpRequested_)};
  engine::readLaunchArguments(simulatorOptions.otherArguments(), launchOptions);
  return options;
}

std::string LaunchOptions::usage() {
  std::string text = engine::usageHeader(PROGRAM_NAME);
  text += serverOptionEntries();
  text += '\n';
  text += network::SimulatorLaunchOptions::usage();
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

std::uint16_t LaunchOptions::port() const { return port_; }

std::optional<std::uint32_t> LaunchOptions::seed() const { return seed_; }

std::size_t LaunchOptions::stateRate() const { return stateRate_; }

std::size_t LaunchOptions::inputDelay() const { return inputDelay_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

const network::NetworkConditions& LaunchOptions::networkConditions() const {
  return networkConditions_;
}

const std::optional<std::filesystem::path>& LaunchOptions::trafficCsvFile()
    const {
  return trafficCsvFile_;
}

LaunchOptions::LaunchOptions(std::uint16_t port, bool helpRequested)
    : port_(port),
      helpRequested_(helpRequested),
      stateRate_(DEFAULT_WORLD_STATES_PER_SECOND),
      inputDelay_(DEFAULT_INPUT_DELAY_TICKS) {}

}  // namespace rtype::server
