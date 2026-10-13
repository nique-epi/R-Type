#include "LaunchOptions.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include "EndpointText.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"

namespace rtype::server {

namespace {

struct LaunchSettings {
  std::uint16_t port;
  bool helpRequested;
};

struct LaunchOption {
  std::string_view name;
  bool takesValue;
  void (*record)(LaunchSettings& settings, std::string_view value);
};

void recordPort(LaunchSettings& settings, std::string_view port) {
  settings.port = network::portFromText(port);
}

void recordHelp(LaunchSettings& settings,
                [[maybe_unused]] std::string_view value) {
  settings.helpRequested = true;
}

constexpr std::array launchOptions{
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
  LaunchSettings settings{.port = network::DEFAULT_SERVER_PORT,
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
  return LaunchOptions{settings.port, settings.helpRequested};
}

std::string LaunchOptions::usage() {
  std::string text =
      "Usage: " + std::string(PROGRAM_NAME) + " [options]\n\nOptions:\n";
  appendUsageEntry(
      text, std::string(PORT_OPTION) + " <port>",
      "UDP port to listen on, from " + std::to_string(network::LOWEST_PORT) +
          " to " + std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_SERVER_PORT) + ".");
  appendUsageEntry(text, HELP_OPTION, "Print this help and start nothing.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

std::string LaunchOptions::usageError(std::string_view message) {
  return std::string(PROGRAM_NAME) + ": " + std::string(message) + "\nRun " +
         std::string(PROGRAM_NAME) + " " + std::string(HELP_OPTION) +
         " to see the options.\n";
}

std::uint16_t LaunchOptions::port() const { return port_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

LaunchOptions::LaunchOptions(std::uint16_t port, bool helpRequested)
    : port_(port), helpRequested_(helpRequested) {}

}  // namespace rtype::server
