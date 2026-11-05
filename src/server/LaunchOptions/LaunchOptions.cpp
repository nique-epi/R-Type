#include "LaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "ExecutablePath.hpp"
#include "ForcedSetting.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"
#include "LaunchValues.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SettingLine.hpp"
#include "SettingOptions.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "SupervisorLink.hpp"
#include "WholeNumberOption.hpp"

namespace rtype::server {

namespace {

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

/**
 * @returns `-p <port>`, which forces the port setting to a checked port.
 */
engine::LaunchOption portOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = engine::PORT_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view port) {
        values.forcedLines.push_back(engine::settingLine(
            PORT_SETTING_NAME, std::to_string(network::portFromText(port))));
      }};
}

/**
 * @returns `--seed <seed>`, which forces the seed setting to a checked seed.
 */
engine::LaunchOption seedOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = SEED_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view seed) {
        values.forcedLines.push_back(engine::settingLine(
            SEED_SETTING_NAME, std::to_string(seedFromText(seed))));
      }};
}

/**
 * @returns `--state-rate <states>`, which forces the state rate setting to a
 * checked rate.
 */
engine::LaunchOption stateRateOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = STATE_RATE_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view rate) {
        values.forcedLines.push_back(engine::settingLine(
            STATE_RATE_SETTING_NAME, std::to_string(stateRateFromText(rate))));
      }};
}

/**
 * @returns `--input-delay <ticks>`, which forces the input delay setting to a
 * checked delay.
 */
engine::LaunchOption inputDelayOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = INPUT_DELAY_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view delay) {
        values.forcedLines.push_back(
            engine::settingLine(INPUT_DELAY_SETTING_NAME,
                                std::to_string(inputDelayFromText(delay))));
      }};
}

/**
 * @returns `--supervisor <port>`, which records the port of the supervisor.
 */
engine::LaunchOption supervisorOption(LaunchValues& values) {
  return engine::LaunchOption{.name = SUPERVISOR_OPTION,
                              .takesValue = true,
                              .record = [&values](std::string_view port) {
                                values.supervisorPort =
                                    network::portFromText(port);
                              }};
}

/**
 * @returns `--link-token <number>`, which records the token shared with the
 * supervisor.
 * @throws engine::InvalidLaunchOptionValueException from the option when the
 * value is 0 or not a whole number.
 */
engine::LaunchOption linkTokenOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = LINK_TOKEN_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view text) {
        const auto token =
            wholeNumberFromText<std::uint64_t>(LINK_TOKEN_OPTION, text);
        if (token == network::NO_SESSION_TOKEN) {
          throw engine::InvalidLaunchOptionValueException(
              std::string(LINK_TOKEN_OPTION), std::string(text));
        }
        values.linkToken = token;
      }};
}

/**
 * @brief Checks that `--supervisor` and `--link-token` came together.
 * @throws IncompleteSupervisorLinkException when only one was given.
 */
void requireCompleteSupervisorLink(const LaunchValues& values) {
  if (values.supervisorPort.has_value() != values.linkToken.has_value()) {
    throw IncompleteSupervisorLinkException(
        std::string{values.supervisorPort.has_value() ? LINK_TOKEN_OPTION
                                                      : SUPERVISOR_OPTION});
  }
}

/** @returns The lines of the usage that describe the options that give a
 * setting a value of its own kind. */
std::string settingOptionEntries() {
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
  return text;
}

/** @returns The line of the usage that describes `--database`. */
std::string databaseOptionEntry() {
  return engine::usageEntry(
      std::string(engine::DATABASE_OPTION) + " <file>",
      "File the leaderboard of this server is kept in, on a local disk. "
      "Default: " +
          std::string(DATABASE_FILE_PREFIX) + "<port>" +
          std::string(DATABASE_FILE_SUFFIX) + " next to the executable.");
}

/** @returns The line of the usage that describes `--mode`. */
std::string modeOptionEntry() {
  return engine::usageEntry(
      std::string(MODE_OPTION) + " <" + std::string(SERVER_MODE_NAME) + "|" +
          std::string(SUPERVISOR_MODE_NAME) + "|" +
          std::string(MASTER_MODE_NAME) + ">",
      "Runs the game server (" + std::string(SERVER_MODE_NAME) +
          ", the default), the supervisor that starts a game server per game "
          "(" +
          std::string(SUPERVISOR_MODE_NAME) +
          ") or the master that lists the game servers (" +
          std::string(MASTER_MODE_NAME) +
          "). It comes before every other option: `" +
          std::string(MODE_OPTION) + " " + std::string(MASTER_MODE_NAME) + " " +
          std::string(engine::HELP_OPTION) +
          "` prints the options of the master.");
}

/** @returns The lines of the usage that describe `--supervisor` and
 * `--link-token`. */
std::string supervisorLinkOptionEntries() {
  std::string text = engine::usageEntry(
      std::string(SUPERVISOR_OPTION) + " <port>",
      "Only for a game process: the UDP port of the supervisor that started "
      "it. Given with " +
          std::string(LINK_TOKEN_OPTION) + ".");
  text += engine::usageEntry(
      std::string(LINK_TOKEN_OPTION) + " <number>",
      "Only for a game process: the number its supervisor gave it.");
  return text;
}

/** @returns The lines of the usage that describe `--set` and `--help`. */
std::string generalOptionEntries() {
  std::string text = engine::setOptionEntry(SERVER_CONFIGURATION_FILE_NAME,
                                            SERVER_SETTING_NAMES);
  text += engine::usageEntry(network::TRAFFIC_CSV_OPTION_WITH_VALUE,
                             network::TRAFFIC_CSV_USAGE);
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  return text;
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options{LaunchValues{.forcedLines = {},
                                     .trafficCsvFile = std::nullopt,
                                     .supervisorPort = std::nullopt,
                                     .linkToken = std::nullopt,
                                     .helpRequested = false}};
  const std::array launchOptions{
      portOption(options.values_),
      seedOption(options.values_),
      stateRateOption(options.values_),
      inputDelayOption(options.values_),
      engine::settingOption(
          engine::ForcedSetting{.option = engine::DATABASE_OPTION,
                                .setting = DATABASE_SETTING_NAME},
          options.values_.forcedLines),
      engine::setOption(options.values_.forcedLines),
      supervisorOption(options.values_),
      linkTokenOption(options.values_),
      engine::pathOption(network::TRAFFIC_CSV_OPTION,
                         options.values_.trafficCsvFile),
      engine::helpOption(options.values_.helpRequested)};
  const std::array<engine::LaunchOptionReader, 2> readers{
      std::bind_front(&network::SimulatorLaunchOptions::readOption,
                      &options.simulatorOptions_),
      std::bind_front(&logging::LogLaunchOptions::readOption,
                      &options.logOptions_)};
  engine::readLaunchArguments(arguments, launchOptions, readers);
  requireCompleteSupervisorLink(options.values_);
  return options;
}

std::string LaunchOptions::usage() {
  std::string text = engine::usageHeader(PROGRAM_NAME);
  text += modeOptionEntry();
  text += settingOptionEntries();
  text += databaseOptionEntry();
  text += supervisorLinkOptionEntries();
  text += generalOptionEntries();
  text += '\n';
  text += network::SimulatorLaunchOptions::usage();
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

const std::vector<std::string>& LaunchOptions::forcedLines() const {
  return values_.forcedLines;
}

bool LaunchOptions::helpRequested() const { return values_.helpRequested; }

const network::NetworkConditions& LaunchOptions::networkConditions() const {
  return simulatorOptions_.conditions();
}

const logging::LogLaunchOptions& LaunchOptions::logOptions() const {
  return logOptions_;
}

std::optional<SupervisorLink> LaunchOptions::supervisorLink() const {
  if (!values_.supervisorPort.has_value() || !values_.linkToken.has_value()) {
    return std::nullopt;
  }
  return SupervisorLink{.port = *values_.supervisorPort,
                        .token = *values_.linkToken};
}

const std::optional<std::filesystem::path>& LaunchOptions::trafficCsvFile()
    const {
  return values_.trafficCsvFile;
}

LaunchOptions::LaunchOptions(LaunchValues values)
    : values_(std::move(values)),
      logOptions_(engine::executableFolder() / logging::SERVER_LOG_FILE_NAME) {}

}  // namespace rtype::server
