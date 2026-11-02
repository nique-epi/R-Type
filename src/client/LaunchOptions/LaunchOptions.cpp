#include "LaunchOptions.hpp"
#include <array>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AudioMode.hpp"
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchValues.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "SettingLine.hpp"
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
        values.serverEndpoint.address = network::ipv4AddressFromText(address);
        values.forcedLines.push_back(engine::settingLine(
            SERVER_ADDRESS_SETTING_NAME, values.serverEndpoint.address));
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
        values.serverEndpoint.port = network::portFromText(port);
        values.forcedLines.push_back(
            engine::settingLine(SERVER_PORT_SETTING_NAME,
                                std::to_string(values.serverEndpoint.port)));
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
        values.nickname = std::string(callsign);
        values.forcedLines.push_back(
            engine::settingLine(NICKNAME_SETTING_NAME, callsign));
      }};
}

/** @returns `--set <name>=<value>`, which forces any setting. */
engine::LaunchOption setOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = engine::SET_OPTION,
      .takesValue = true,
      .record = [&values](std::string_view assignment) {
        values.forcedLines.push_back(
            engine::settingLineFromSetOption(assignment));
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

/** @returns `--help`, which asks for the usage. */
engine::LaunchOption helpOption(LaunchValues& values) {
  return engine::LaunchOption{
      .name = engine::HELP_OPTION,
      .takesValue = false,
      .record = [&values]([[maybe_unused]] std::string_view value) {
        values.helpRequested = true;
      }};
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchValues values{
      .serverEndpoint =
          network::Endpoint{.address = std::string(DEFAULT_SERVER_ADDRESS),
                            .port = network::DEFAULT_SERVER_PORT},
      .nickname = std::string(DEFAULT_NICKNAME),
      .audioMode = AudioMode::Enabled,
      .forcedLines = {},
      .helpRequested = false};
  const std::array launchOptions{addressOption(values),  portOption(values),
                                 nicknameOption(values), setOption(values),
                                 noSoundOption(values),  helpOption(values)};
  engine::readLaunchArguments(arguments, launchOptions);
  return LaunchOptions{std::move(values)};
}

std::string LaunchOptions::usage() {
  std::string text = engine::usageHeader(PROGRAM_NAME);
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
      std::string(engine::SET_OPTION) + " <name>=<value>",
      "Gives a setting a value, as the line `set <name> <value>` of " +
          std::string(CLIENT_CONFIGURATION_FILE_NAME) +
          " does. It wins over that file and can be given several times. "
          "Settings: " +
          std::string(NICKNAME_SETTING_NAME) + ", " +
          std::string(SERVER_ADDRESS_SETTING_NAME) + ", " +
          std::string(SERVER_PORT_SETTING_NAME) + ".");
  text += engine::usageEntry(std::string(NO_SOUND_OPTION),
                             "Start the client without sound.");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and open no window.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::CLIENT_LOG_FILE_NAME);
  return text;
}

const network::Endpoint& LaunchOptions::serverEndpoint() const {
  return values_.serverEndpoint;
}

const std::vector<std::string>& LaunchOptions::forcedLines() const {
  return values_.forcedLines;
}

const std::string& LaunchOptions::nickname() const { return values_.nickname; }

AudioMode LaunchOptions::audioMode() const { return values_.audioMode; }

bool LaunchOptions::helpRequested() const { return values_.helpRequested; }

LaunchOptions::LaunchOptions(LaunchValues values)
    : values_(std::move(values)) {}

}  // namespace rtype::client
