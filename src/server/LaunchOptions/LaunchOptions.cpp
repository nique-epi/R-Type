#include "LaunchOptions.hpp"
#include <array>
#include <charconv>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The whole number @p text is made of, digits only.
 * @throws engine::InvalidLaunchOptionValueException when it is anything else,
 * or does not fit in 32 bits.
 */
std::uint32_t seedFromText(std::string_view text) {
  std::uint32_t seed = 0;
  const std::string digits{text};
  const char* const end = digits.data() + digits.size();
  const auto [stop, error] = std::from_chars(digits.data(), end, seed);
  if (digits.empty() || error != std::errc{} || stop != end) {
    throw engine::InvalidLaunchOptionValueException(std::string(SEED_OPTION),
                                                    std::string(text));
  }
  return seed;
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options{network::DEFAULT_SERVER_PORT, false};
  const std::array launchOptions{
      engine::LaunchOption{.name = engine::PORT_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view port) {
                                 options.port_ = network::portFromText(port);
                               }},
      engine::LaunchOption{.name = SEED_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view seed) {
                                 options.seed_ = seedFromText(seed);
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
  text += engine::usageEntry(
      std::string(SEED_OPTION) + " <seed>",
      "Whole number the random draws of the games start from, from 0 to "
      "4294967295. Default: drawn at random, and logged when a game starts so "
      "it can be given here to replay it.");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

std::uint16_t LaunchOptions::port() const { return port_; }

std::optional<std::uint32_t> LaunchOptions::seed() const { return seed_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

LaunchOptions::LaunchOptions(std::uint16_t port, bool helpRequested)
    : port_(port), helpRequested_(helpRequested) {}

}  // namespace rtype::server
