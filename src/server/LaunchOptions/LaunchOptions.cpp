#include "LaunchOptions.hpp"
#include <array>
#include <charconv>
#include <cstddef>
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
 * @returns The input delay @p text writes, from LOWEST_INPUT_DELAY_TICKS to
 * HIGHEST_INPUT_DELAY_TICKS.
 * @throws engine::InvalidLaunchOptionValueException when it is not one.
 */
std::size_t inputDelayFromText(std::string_view text) {
  const auto delay = wholeNumberFromText<std::size_t>(INPUT_DELAY_OPTION, text);
  if (delay < LOWEST_INPUT_DELAY_TICKS || delay > HIGHEST_INPUT_DELAY_TICKS) {
    throw engine::InvalidLaunchOptionValueException(
        std::string(INPUT_DELAY_OPTION), std::string(text));
  }
  return delay;
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
      engine::LaunchOption{.name = STATE_RATE_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view rate) {
                                 options.stateRate_ = stateRateFromText(rate);
                               }},
      engine::LaunchOption{.name = INPUT_DELAY_OPTION,
                           .takesValue = true,
                           .record =
                               [&options](std::string_view delay) {
                                 options.inputDelay_ =
                                     inputDelayFromText(delay);
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
      std::string(INPUT_DELAY_OPTION) + " <ticks>",
      "Ticks every player's input is held before it is applied, from " +
          std::to_string(LOWEST_INPUT_DELAY_TICKS) + " to " +
          std::to_string(HIGHEST_INPUT_DELAY_TICKS) +
          ". The same delay holds for every player, so it adds the same lag to "
          "all of them. Default: " +
          std::to_string(DEFAULT_INPUT_DELAY_TICKS) + ".");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start nothing.");
  text += '\n';
  text += logging::LogLaunchOptions::usage(logging::SERVER_LOG_FILE_NAME);
  return text;
}

std::uint16_t LaunchOptions::port() const { return port_; }

std::optional<std::uint32_t> LaunchOptions::seed() const { return seed_; }

std::size_t LaunchOptions::stateRate() const { return stateRate_; }

std::size_t LaunchOptions::inputDelay() const { return inputDelay_; }

bool LaunchOptions::helpRequested() const { return helpRequested_; }

LaunchOptions::LaunchOptions(std::uint16_t port, bool helpRequested)
    : port_(port),
      helpRequested_(helpRequested),
      stateRate_(DEFAULT_WORLD_STATES_PER_SECOND),
      inputDelay_(DEFAULT_INPUT_DELAY_TICKS) {}

}  // namespace rtype::server
