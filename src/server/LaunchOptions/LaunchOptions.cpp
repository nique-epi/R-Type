#include "LaunchOptions.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include "EndpointText.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options{network::DEFAULT_SERVER_PORT, false};
  const std::array launchOptions{
      engine::LaunchOption{.name = engine::PORT_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view port) {
                                 options.port_ = network::portFromText(port);
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
  text += engine::usageEntry(
      std::string(engine::PORT_OPTION) + " <port>",
      "UDP port to listen on, from " + std::to_string(network::LOWEST_PORT) +
          " to " + std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_SERVER_PORT) + ".");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

std::uint16_t LaunchOptions::port() const { return port_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

LaunchOptions::LaunchOptions(std::uint16_t port, bool helpRequested)
    : port_(port), helpRequested_(helpRequested) {}

}  // namespace rtype::server
