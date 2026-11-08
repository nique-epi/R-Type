#include "LaunchOptions.hpp"
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
#include "AudioMode.hpp"
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "ExecutablePath.hpp"
#include "JournalFolder.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"
#include "LaunchValues.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "PathText.hpp"
#include "PlayStart.hpp"
#include "ProtocolConstants.hpp"
#include "ServerChoice.hpp"
#include "SettingLine.hpp"
#include "SettingOptions.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "WholeNumberText.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/**
 * @returns `-h <address>`, which sets the address of the server and forces the
 * setting of the same name.
 */
engine::LaunchOption addressOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = ADDRESS_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view address) {
        values.choice.server.address = network::ipv4AddressFromText(address);
        values.forcedLines.push_back(engine::settingLine(
            SERVER_ADDRESS_SETTING_NAME, values.choice.server.address));
        values.playStart = PlayStart::GivenServer;
      }};
}

/**
 * @returns `-p <port>`, which sets the port of the server and forces the
 * setting of the same name.
 */
engine::LaunchOption portOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = engine::PORT_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view port) {
        values.choice.server.port = network::portFromText(port);
        values.forcedLines.push_back(
            engine::settingLine(SERVER_PORT_SETTING_NAME,
                                std::to_string(values.choice.server.port)));
      }};
}

/**
 * @returns `-n <nickname>`, which sets the nickname and forces the setting of
 * the same name.
 */
engine::LaunchOption nicknameOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = NICKNAME_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view callsign) {
        if (!network::isValidNickname(callsign)) {
          throw InvalidNicknameOptionException(callsign);
        }
        values.choice.nickname = std::string(callsign);
        values.forcedLines.push_back(
            engine::settingLine(NICKNAME_SETTING_NAME, callsign));
      }};
}

/** @returns The game @p text names after `--game`: NEW_GAME_VALUE for a new
 * game, else a number from network::FIRST_HOSTED_GAME_NUMBER to 65535 for that
 * game.
 * @throws InvalidGameOptionException for any other text. */
network::GameChoice gameChoiceFromText(std::string_view text) {
  if (text == NEW_GAME_VALUE) {
    return network::NEW_GAME_CHOICE;
  }
  const std::optional<std::uint16_t> number =
      network::wholeNumberIn<std::uint16_t>(text);
  if (!number.has_value() || *number < network::FIRST_HOSTED_GAME_NUMBER) {
    throw InvalidGameOptionException(text);
  }
  return network::GameChoice{.kind = network::GameChoiceKind::ExistingGame,
                             .number = *number};
}

/** @returns `--game <game>`, which sets the game the client asks for. */
engine::LaunchOption gameOption(LaunchValues& values) {
  return engine::LaunchOption{.name = GAME_OPTION,
                              .takesValue = true,
                              .record = [&values](std::string_view game) {
                                values.choice.game = gameChoiceFromText(game);
                              }};
}

/** @returns `--no-sound`, which starts the client without sound. */
engine::LaunchOption noSoundOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = NO_SOUND_OPTION,
      .takesValue = false,
      .record = [&values]([[maybe_unused]] std::string_view value) {
        values.audioMode = AudioMode::Disabled;
      }};
}

/** @returns The lines of the usage that describe the options that name the
 * server to join, the player and the settings. */
std::string connectionOptionEntries() {
  std::string text;
  text += engine::usageEntry(std::string(ADDRESS_OPTION) + " <address>",
                             "IPv4 address of the server, such as " +
                                 std::string(network::LOOPBACK_IPV4_ADDRESS) +
                                 ". Host names are not resolved. Default: " +
                                 std::string(DEFAULT_SERVER_ADDRESS) + ".");
  text += engine::usageEntry(
      std::string(engine::PORT_OPTION) + " <port>",
      "UDP port of the server, from " + std::to_string(network::LOWEST_PORT) +
          " to " + std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_SERVER_PORT) + ".");
  text += engine::usageEntry(
      std::string(NICKNAME_OPTION) + " <nickname>",
      "Callsign of the player, " +
          std::to_string(network::MINIMUM_NICKNAME_SIZE) + " to " +
          std::to_string(network::MAXIMUM_NICKNAME_SIZE) +
          " bytes of UTF-8. Default: " + std::string(DEFAULT_NICKNAME) + ".");
  text += engine::usageEntry(
      std::string(GAME_OPTION) + " <game>",
      "Game to ask the server for: " + std::string(NEW_GAME_VALUE) +
          " opens one, a number from " +
          std::to_string(network::FIRST_HOSTED_GAME_NUMBER) +
          " joins the game of that number. Default: any game with a free "
          "seat.");
  text += engine::setOptionEntry(CLIENT_CONFIGURATION_FILE_NAME,
                                 CLIENT_SETTING_NAMES);
  return text;
}

std::optional<std::filesystem::path> journalInItsFolder() {
  const std::optional<std::filesystem::path> folder = journalFolder();
  if (!folder.has_value()) {
    return std::nullopt;
  }
  return *folder / logging::CLIENT_LOG_FILE_NAME;
}

std::filesystem::path defaultJournal() {
  const std::optional<std::filesystem::path> journal = journalInItsFolder();
  if (journal.has_value()) {
    return *journal;
  }
  return engine::executableFolder() / logging::CLIENT_LOG_FILE_NAME;
}

std::string journalUsage() {
  const std::optional<std::filesystem::path> journal = journalInItsFolder();
  if (journal.has_value()) {
    return logging::LogLaunchOptions::usageWithJournalAt(
        logging::genericText(*journal));
  }
  return logging::LogLaunchOptions::usage(logging::CLIENT_LOG_FILE_NAME);
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options{LaunchValues{
      .choice =
          ServerChoice{.server =
                           network::Endpoint{
                               .address = std::string(DEFAULT_SERVER_ADDRESS),
                               .port = network::DEFAULT_SERVER_PORT},
                       .nickname = std::string(DEFAULT_NICKNAME),
                       .game = network::ANY_GAME_CHOICE},
      .playStart = PlayStart::ServerMap,
      .audioMode = AudioMode::Enabled,
      .trafficCsvFile = std::nullopt,
      .forcedLines = {},
      .helpRequested = false}};
  const std::array launchOptions{
      addressOption(options.values_),
      portOption(options.values_),
      nicknameOption(options.values_),
      gameOption(options.values_),
      engine::setOption(options.values_.forcedLines),
      noSoundOption(options.values_),
      engine::pathOption(network::TRAFFIC_CSV_OPTION,
                         options.values_.trafficCsvFile),
      engine::helpOption(options.values_.helpRequested)};
  const std::array<engine::LaunchOptionReader, 2> readers{
      std::bind_front(&network::SimulatorLaunchOptions::readOption,
                      &options.simulatorOptions_),
      std::bind_front(&logging::LogLaunchOptions::readOption,
                      &options.logOptions_)};
  engine::readLaunchArguments(arguments, launchOptions, readers);
  return options;
}

std::string LaunchOptions::usage() {
  std::string text = engine::usageHeader(PROGRAM_NAME);
  text += connectionOptionEntries();
  text += engine::usageEntry(std::string(NO_SOUND_OPTION),
                             "Start the client without sound.");
  text += engine::usageEntry(network::TRAFFIC_CSV_OPTION_WITH_VALUE,
                             network::TRAFFIC_CSV_USAGE);
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and open no window.");
  text += '\n';
  text += network::SimulatorLaunchOptions::usage();
  text += '\n';
  text += journalUsage();
  return text;
}

const ServerChoice& LaunchOptions::serverChoice() const {
  return values_.choice;
}

PlayStart LaunchOptions::playStart() const { return values_.playStart; }

const std::vector<std::string>& LaunchOptions::forcedLines() const {
  return values_.forcedLines;
}

AudioMode LaunchOptions::audioMode() const { return values_.audioMode; }

bool LaunchOptions::helpRequested() const { return values_.helpRequested; }

const network::NetworkConditions& LaunchOptions::networkConditions() const {
  return simulatorOptions_.conditions();
}

const logging::LogLaunchOptions& LaunchOptions::logOptions() const {
  return logOptions_;
}

const std::optional<std::filesystem::path>& LaunchOptions::trafficCsvFile()
    const {
  return values_.trafficCsvFile;
}

LaunchOptions::LaunchOptions(LaunchValues values)
    : values_(std::move(values)), logOptions_(defaultJournal()) {}

}  // namespace rtype::client
