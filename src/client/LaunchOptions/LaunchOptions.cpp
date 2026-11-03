#include "LaunchOptions.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "AudioMode.hpp"
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "ExecutablePath.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LaunchOptionReader.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "SimulatorLaunchOptions.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

namespace {

/**
 * @returns `-h <address>`, which writes a checked IPv4 address into
 * @p address.
 */
engine::LaunchOption addressOption(std::string& address) {
  return engine::LaunchOption{.name = ADDRESS_OPTION,
                              .takesValue = true,
                              .record = [&address](std::string_view text) {
                                address = network::ipv4AddressFromText(text);
                              }};
}

/** @returns `-p <port>`, which writes a checked port into @p port. */
engine::LaunchOption portOption(std::uint16_t& port) {
  return engine::LaunchOption{.name = engine::PORT_OPTION,
                              .takesValue = true,
                              .record = [&port](std::string_view text) {
                                port = network::portFromText(text);
                              }};
}

/**
 * @returns `-n <nickname>`, which writes the nickname into @p nickname.
 * Its record throws InvalidNicknameOptionException for a nickname the
 * protocol refuses.
 */
engine::LaunchOption nicknameOption(std::string& nickname) {
  return engine::LaunchOption{
      .name = NICKNAME_OPTION,
      .takesValue = true,
      .record = [&nickname](std::string_view callsign) {
        if (!network::isValidNickname(callsign)) {
          throw InvalidNicknameOptionException(callsign);
        }
        nickname = std::string(callsign);
      }};
}

/** @returns `--no-sound`, which writes AudioMode::Disabled into @p mode. */
engine::LaunchOption noSoundOption(AudioMode& mode) {
  return engine::LaunchOption{
      .name = NO_SOUND_OPTION,
      .takesValue = false,
      .record = [&mode]([[maybe_unused]] std::string_view value) {
        mode = AudioMode::Disabled;
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

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options{
      network::Endpoint{.address = std::string(DEFAULT_SERVER_ADDRESS),
                        .port = network::DEFAULT_SERVER_PORT},
      std::string(DEFAULT_NICKNAME), false};
  const std::array launchOptions{addressOption(options.serverEndpoint_.address),
                                 portOption(options.serverEndpoint_.port),
                                 nicknameOption(options.nickname_),
                                 noSoundOption(options.audioMode_),
                                 helpOption(options.helpRequested_)};
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
  text += engine::usageEntry(std::string(NO_SOUND_OPTION),
                             "Start the client without sound.");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and open no window.");
  text += '\n';
  text += network::SimulatorLaunchOptions::usage();
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::CLIENT_LOG_FILE_NAME);
  return text;
}

const network::Endpoint& LaunchOptions::serverEndpoint() const {
  return serverEndpoint_;
}

const std::string& LaunchOptions::nickname() const { return nickname_; }

AudioMode LaunchOptions::audioMode() const { return audioMode_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

const network::NetworkConditions& LaunchOptions::networkConditions() const {
  return simulatorOptions_.conditions();
}

const logging::LogLaunchOptions& LaunchOptions::logOptions() const {
  return logOptions_;
}

LaunchOptions::LaunchOptions(network::Endpoint serverEndpoint,
                             std::string nickname, bool helpRequested)
    : serverEndpoint_(std::move(serverEndpoint)),
      nickname_(std::move(nickname)),
      helpRequested_(helpRequested),
      logOptions_(engine::executableFolder() / logging::CLIENT_LOG_FILE_NAME) {}

}  // namespace rtype::client
