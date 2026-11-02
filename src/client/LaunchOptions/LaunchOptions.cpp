#include "LaunchOptions.hpp"
#include <array>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "AudioMode.hpp"
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ProtocolConstants.hpp"
#include "WireTypes.hpp"

namespace rtype::client {

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options{
      network::Endpoint{.address = std::string(DEFAULT_SERVER_ADDRESS),
                        .port = network::DEFAULT_SERVER_PORT},
      std::string(DEFAULT_NICKNAME), false};
  const std::array launchOptions{
      engine::LaunchOption{.name = ADDRESS_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view address) {
                                 options.serverEndpoint_.address =
                                     network::ipv4AddressFromText(address);
                               }},
      engine::LaunchOption{.name = engine::PORT_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view port) {
                                 options.serverEndpoint_.port =
                                     network::portFromText(port);
                               }},
      engine::LaunchOption{.name = NICKNAME_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view nickname) {
                                 if (!network::isValidNickname(nickname)) {
                                   throw InvalidNicknameOptionException(
                                       nickname);
                                 }
                                 options.nickname_ = std::string(nickname);
                               }},
      engine::LaunchOption{
          .name = NO_SOUND_OPTION,
          .takesValue = false,
          .record =
              [&options]([[maybe_unused]] std::string_view value) {
                options.audioMode_ = AudioMode::Disabled;
              }},
      engine::LaunchOption{
          .name = engine::HELP_OPTION,
          .takesValue = false,
          .record =
              [&options]([[maybe_unused]] std::string_view value) {
                options.helpRequested_ = true;
              }},
  };
  engine::readLaunchArguments(arguments, launchOptions);
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
  text += logging::LogLaunchOptions::usage(logging::CLIENT_LOG_FILE_NAME);
  return text;
}

const network::Endpoint& LaunchOptions::serverEndpoint() const {
  return serverEndpoint_;
}

const std::string& LaunchOptions::nickname() const { return nickname_; }

AudioMode LaunchOptions::audioMode() const { return audioMode_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

LaunchOptions::LaunchOptions(network::Endpoint serverEndpoint,
                             std::string nickname, bool helpRequested)
    : serverEndpoint_(std::move(serverEndpoint)),
      nickname_(std::move(nickname)),
      helpRequested_(helpRequested) {}

}  // namespace rtype::client
