#include "LaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include "ClientConstants.hpp"
#include "ClientException.hpp"
#include "Endpoint.hpp"
#include "EndpointText.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"

namespace rtype::client {

namespace {

struct LaunchSettings {
  network::Endpoint serverEndpoint;
  bool helpRequested{false};
};

struct LaunchOption {
  std::string_view name;
  bool takesValue;
  void (*record)(LaunchSettings& settings, std::string_view value);
};

void recordAddress(LaunchSettings& settings, std::string_view address) {
  settings.serverEndpoint.address = network::ipv4AddressFromText(address);
}

void recordPort(LaunchSettings& settings, std::string_view port) {
  settings.serverEndpoint.port = network::portFromText(port);
}

void recordHelp(LaunchSettings& settings,
                [[maybe_unused]] std::string_view value) {
  settings.helpRequested = true;
}

constexpr std::array launchOptions{
    LaunchOption{
        .name = ADDRESS_OPTION, .takesValue = true, .record = recordAddress},
    LaunchOption{.name = PORT_OPTION, .takesValue = true, .record = recordPort},
    LaunchOption{
        .name = HELP_OPTION, .takesValue = false, .record = recordHelp},
};

std::optional<LaunchOption> findLaunchOption(std::string_view argument) {
  for (const LaunchOption& option : launchOptions) {
    if (option.name == argument) {
      return option;
    }
  }
  return std::nullopt;
}

std::string_view valueAfter(std::span<const std::string_view> arguments,
                            std::size_t& index) {
  const std::size_t valueIndex = index + 1;
  if (valueIndex >= arguments.size()) {
    throw MissingLaunchOptionValueException(std::string(arguments[index]));
  }
  index = valueIndex;
  return arguments[valueIndex];
}

void appendUsageEntry(std::string& text, std::string_view option,
                      std::string_view description) {
  text += "  ";
  text += option;
  text += "\n      ";
  text += description;
  text += '\n';
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchSettings settings{
      .serverEndpoint =
          network::Endpoint{.address = std::string(DEFAULT_SERVER_ADDRESS),
                            .port = network::DEFAULT_SERVER_PORT},
      .helpRequested = false};
  for (std::size_t index = 0; index < arguments.size(); ++index) {
    const std::optional<LaunchOption> option =
        findLaunchOption(arguments[index]);
    if (!option.has_value()) {
      throw UnknownLaunchArgumentException(std::string(arguments[index]));
    }
    const std::string_view value =
        option->takesValue ? valueAfter(arguments, index) : std::string_view{};
    option->record(settings, value);
  }
  return LaunchOptions{std::move(settings.serverEndpoint),
                       settings.helpRequested};
}

std::string LaunchOptions::usage() {
  std::string text =
      "Usage: " + std::string(PROGRAM_NAME) + " [options]\n\nOptions:\n";
  appendUsageEntry(text, std::string(ADDRESS_OPTION) + " <address>",
                   "IPv4 address of the server, such as " +
                       std::string(network::LOOPBACK_IPV4_ADDRESS) +
                       ". Host names are not resolved. Default: " +
                       std::string(DEFAULT_SERVER_ADDRESS) + ".");
  appendUsageEntry(
      text, std::string(PORT_OPTION) + " <port>",
      "UDP port of the server, from " + std::to_string(network::LOWEST_PORT) +
          " to " + std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_SERVER_PORT) + ".");
  appendUsageEntry(text, HELP_OPTION, "Print this help and open no window.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::CLIENT_LOG_FILE_NAME);
  return text;
}

std::string LaunchOptions::usageError(std::string_view message) {
  return std::string(PROGRAM_NAME) + ": " + std::string(message) + "\nRun " +
         std::string(PROGRAM_NAME) + " " + std::string(HELP_OPTION) +
         " to see the options.\n";
}

const network::Endpoint& LaunchOptions::serverEndpoint() const {
  return serverEndpoint_;
}

bool LaunchOptions::helpRequested() const { return helpRequested_; }

LaunchOptions::LaunchOptions(network::Endpoint serverEndpoint,
                             bool helpRequested)
    : serverEndpoint_(std::move(serverEndpoint)),
      helpRequested_(helpRequested) {}

}  // namespace rtype::client
