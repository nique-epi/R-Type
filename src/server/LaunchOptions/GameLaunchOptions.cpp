#include "GameLaunchOptions.hpp"
#include <array>
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
#include "GameLaunchValues.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "ServerLaunchOptionParts.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "SupervisorLink.hpp"
#include "WholeNumberOption.hpp"

namespace rtype::server {

namespace {

/**
 * @returns `--supervisor <port>`, which records the port of the supervisor.
 */
engine::LaunchOption supervisorOption(GameLaunchValues& values) {
  return engine::LaunchOption{.name = SUPERVISOR_OPTION,
                              .takesValue = true,
                              .record = [&values](std::string_view port) {
                                values.supervisorLink.port =
                                    network::portFromText(port);
                              }};
}

/**
 * @returns `--link-token <number>`, which records the token shared with the
 * supervisor.
 * @throws engine::InvalidLaunchOptionValueException from the option when the
 * value is 0 or not a whole number.
 */
engine::LaunchOption linkTokenOption(GameLaunchValues& values) {
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
        values.supervisorLink.token = token;
      }};
}

/**
 * @brief Checks that a game process that does not only print its usage was
 * given both `--supervisor` and `--link-token`.
 * @throws MissingSupervisorLinkException naming the first one missing.
 */
void requireSupervisorLink(const GameLaunchValues& values) {
  if (values.helpRequested) {
    return;
  }
  if (values.supervisorLink.port == 0) {
    throw MissingSupervisorLinkException(std::string{SUPERVISOR_OPTION});
  }
  if (values.supervisorLink.token == network::NO_SESSION_TOKEN) {
    throw MissingSupervisorLinkException(std::string{LINK_TOKEN_OPTION});
  }
}

/** @returns The lines of the usage that describe `--supervisor` and
 * `--link-token`. */
std::string supervisorLinkOptionEntries() {
  std::string text = engine::usageEntry(
      std::string(SUPERVISOR_OPTION) + " <port>",
      "The UDP port of the supervisor that started this game. Required.");
  text += engine::usageEntry(
      std::string(LINK_TOKEN_OPTION) + " <number>",
      "The number its supervisor gave this game, never 0. Required.");
  return text;
}

/** @returns The line of the usage that describes `--configuration`. */
std::string configurationOptionEntry() {
  return engine::usageEntry(
      std::string(CONFIGURATION_OPTION) + " <file>",
      "File of commands this game reads when it starts, written by its "
      "supervisor from the lines of " +
          std::string(SERVER_CONFIGURATION_FILE_NAME) +
          " that belong to the games. Default: no file.");
}

}  // namespace

GameLaunchOptions GameLaunchOptions::read(
    std::span<const std::string_view> arguments) {
  GameLaunchOptions options{GameLaunchValues{.forcedLines = {},
                                             .trafficCsvFile = std::nullopt,
                                             .configurationFile = std::nullopt,
                                             .supervisorLink = {},
                                             .helpRequested = false}};
  const std::array launchOptions{
      portOption(options.values_.forcedLines),
      databaseOption(options.values_.forcedLines),
      difficultyOption(options.values_.forcedLines),
      livesOption(options.values_.forcedLines),
      friendlyFireOption(options.values_.forcedLines),
      supervisorOption(options.values_),
      linkTokenOption(options.values_),
      engine::pathOption(CONFIGURATION_OPTION,
                         options.values_.configurationFile),
      engine::pathOption(network::TRAFFIC_CSV_OPTION,
                         options.values_.trafficCsvFile),
      engine::helpOption(options.values_.helpRequested)};
  const std::array<engine::LaunchOptionReader, 2> readers{
      std::bind_front(&network::SimulatorLaunchOptions::readOption,
                      &options.simulatorOptions_),
      std::bind_front(&logging::LogLaunchOptions::readOption,
                      &options.logOptions_)};
  engine::readLaunchArguments(arguments, launchOptions, readers);
  requireSupervisorLink(options.values_);
  return options;
}

std::string GameLaunchOptions::usage() {
  std::string text = engine::usageHeader(PROGRAM_NAME);
  text += portOptionEntry();
  text += supervisorLinkOptionEntries();
  text += databaseOptionEntry();
  text += ruleOptionEntries();
  text += configurationOptionEntry();
  text += engine::usageEntry(network::TRAFFIC_CSV_OPTION_WITH_VALUE,
                             network::TRAFFIC_CSV_USAGE);
  text += helpOptionEntry();
  text += '\n';
  text += network::SimulatorLaunchOptions::usage();
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

const std::vector<std::string>& GameLaunchOptions::forcedLines() const {
  return values_.forcedLines;
}

SupervisorLink GameLaunchOptions::supervisorLink() const {
  return values_.supervisorLink;
}

bool GameLaunchOptions::helpRequested() const { return values_.helpRequested; }

const network::NetworkConditions& GameLaunchOptions::networkConditions() const {
  return simulatorOptions_.conditions();
}

const logging::LogLaunchOptions& GameLaunchOptions::logOptions() const {
  return logOptions_;
}

const std::optional<std::filesystem::path>& GameLaunchOptions::trafficCsvFile()
    const {
  return values_.trafficCsvFile;
}

const std::optional<std::filesystem::path>&
GameLaunchOptions::configurationFile() const {
  return values_.configurationFile;
}

GameLaunchOptions::GameLaunchOptions(GameLaunchValues values)
    : values_(std::move(values)),
      logOptions_(engine::executableFolder() / logging::SERVER_LOG_FILE_NAME) {}

}  // namespace rtype::server
