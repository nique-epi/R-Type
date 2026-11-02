#include "LaunchOptions.hpp"
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "LogLaunchOptions.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "SettingLine.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The whole number @p text is made of, digits only.
 * @throws engine::InvalidLaunchOptionValueException naming @p option when it
 * is anything else, or does not fit in a Number.
 */
template <typename Number>
Number wholeNumberFromText(std::string_view option, std::string_view text) {
  Number number = 0;
  const std::string digits{text};
  const char* const end = digits.data() + digits.size();
  const auto [stop, error] = std::from_chars(digits.data(), end, number);
  if (digits.empty() || error != std::errc{} || stop != end) {
    throw engine::InvalidLaunchOptionValueException(std::string(option),
                                                    std::string(text));
  }
  return number;
}

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
 * @returns The line that gives the setting @p assignment names its value.
 * @throws engine::InvalidLaunchOptionValueException naming @p option when
 * @p assignment is not `name=value` with a value holding no quote.
 */
std::string settingLineFrom(std::string_view option,
                            std::string_view assignment) {
  try {
    return engine::settingLineFromAssignment(assignment);
  } catch (const engine::CommandRefusedException&) {
    throw engine::InvalidLaunchOptionValueException(std::string(option),
                                                    std::string(assignment));
  }
}

}  // namespace

LaunchOptions LaunchOptions::read(std::span<const std::string_view> arguments) {
  LaunchOptions options;
  const std::array launchOptions{
      engine::LaunchOption{
          .name = engine::PORT_OPTION,
          .takesValue = true,
          .record =
              [&options](std::string_view port) {
                options.forcedLines_.push_back(engine::settingLine(
                    PORT_SETTING_NAME,
                    std::to_string(network::portFromText(port))));
              }},
      engine::LaunchOption{
          .name = SEED_OPTION,
          .takesValue = true,
          .record =
              [&options](std::string_view seed) {
                options.forcedLines_.push_back(engine::settingLine(
                    SEED_SETTING_NAME, std::to_string(seedFromText(seed))));
              }},
      engine::LaunchOption{
          .name = STATE_RATE_OPTION,
          .takesValue = true,
          .record =
              [&options](std::string_view rate) {
                options.forcedLines_.push_back(engine::settingLine(
                    STATE_RATE_SETTING_NAME,
                    std::to_string(stateRateFromText(rate))));
              }},
      engine::LaunchOption{.name = engine::SET_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view assignment) {
                                 options.forcedLines_.push_back(settingLineFrom(
                                     engine::SET_OPTION, assignment));
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
  text += engine::usageEntry(
      std::string(STATE_RATE_OPTION) + " <states>",
      "World states sent to each player per second, from " +
          std::to_string(LOWEST_WORLD_STATES_PER_SECOND) + " to " +
          std::to_string(HIGHEST_WORLD_STATES_PER_SECOND) +
          ". More states move the other ships more smoothly and use more "
          "bandwidth. Default: " +
          std::to_string(DEFAULT_WORLD_STATES_PER_SECOND) + ".");
  text += engine::usageEntry(
      std::string(engine::SET_OPTION) + " <name>=<value>",
      "Gives a setting a value, as the line `set <name> <value>` of " +
          std::string(SERVER_CONFIGURATION_FILE_NAME) +
          " does. It wins over that file and can be given several times. "
          "Settings: " +
          std::string(PORT_SETTING_NAME) + ", " +
          std::string(SEED_SETTING_NAME) + ", " +
          std::string(STATE_RATE_SETTING_NAME) + ".");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

const std::vector<std::string>& LaunchOptions::forcedLines() const {
  return forcedLines_;
}

bool LaunchOptions::helpRequested() const { return helpRequested_; }

}  // namespace rtype::server
