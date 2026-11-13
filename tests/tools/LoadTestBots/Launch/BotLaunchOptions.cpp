#include "BotLaunchOptions.hpp"
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include "BotLaunch.hpp"
#include "BotLaunchConstants.hpp"
#include "BotPilotKind.hpp"
#include "ClientConstants.hpp"
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "NetworkConstants.hpp"
#include "WholeNumberOption.hpp"

namespace rtype::tools {

namespace {

/** @brief Refuses @p text as the value of @p option. */
[[noreturn]] void refuse(std::string_view option, std::string_view text) {
  throw engine::InvalidLaunchOptionValueException(std::string(option),
                                                  std::string(text));
}

/** @returns The count @p text writes, from 1 to MAXIMUM_BOT_COUNT. */
std::size_t botCountFrom(std::string_view text) {
  const auto count =
      server::wholeNumberFromText<std::size_t>(BOTS_OPTION, text);
  if (count == 0 || count > MAXIMUM_BOT_COUNT) {
    refuse(BOTS_OPTION, text);
  }
  return count;
}

/** @returns The duration @p text writes in seconds, from 1 to 4294967295. */
std::chrono::seconds durationFrom(std::string_view text) {
  const auto seconds =
      server::wholeNumberFromText<std::uint32_t>(SECONDS_OPTION, text);
  if (seconds == 0) {
    refuse(SECONDS_OPTION, text);
  }
  return std::chrono::seconds{seconds};
}

/** @returns The seed @p text writes, from 0 to 4294967295. */
std::uint32_t seedFrom(std::string_view text) {
  return server::wholeNumberFromText<std::uint32_t>(SEED_OPTION, text);
}

/**
 * @returns The option @p name, followed by a value: it writes into @p field
 * what @p read returns for that value, and what @p read throws refuses it.
 */
template <typename Value>
engine::LaunchOption valueOption(std::string_view name, Value& field,
                                 Value (*read)(std::string_view)) {
  return engine::LaunchOption{
      .name = name,
      .takesValue = true,
      .record = [&field, read](std::string_view text) { field = read(text); }};
}

/** @returns The option @p name, which has @p kind fly the bots. */
engine::LaunchOption pilotOption(std::string_view name, BotLaunch& launch,
                                 BotPilotKind kind) {
  return engine::LaunchOption{
      .name = name,
      .takesValue = false,
      .record = [&launch, kind]([[maybe_unused]] std::string_view value) {
        launch.pilot = kind;
      }};
}

/** @returns The lines of the usage about what flies the bots. */
std::string pilotUsage() {
  return engine::usageEntry(RULES_OPTION, "Fly the bots with simple rules.");
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

BotLaunch readBotLaunch(std::span<const std::string_view> arguments) {
  BotLaunch launch;
  const std::array options{
      valueOption(client::ADDRESS_OPTION, launch.server.address,
                  network::ipv4AddressFromText),
      valueOption(engine::PORT_OPTION, launch.server.port,
                  network::portFromText),
      valueOption(BOTS_OPTION, launch.botCount, botCountFrom),
      valueOption(SECONDS_OPTION, launch.duration, durationFrom),
      valueOption(SEED_OPTION, launch.seed, seedFrom),
      pilotOption(RULES_OPTION, launch, BotPilotKind::Rules),
      helpOption(launch.helpRequested)};
  engine::readLaunchArguments(arguments, options);
  return launch;
}

std::string botUsage() {
  std::string text = engine::usageHeader(BOTS_PROGRAM_NAME);
  text +=
      engine::usageEntry(std::string(client::ADDRESS_OPTION) + " <address>",
                         "IPv4 address of the server to join. Default: " +
                             std::string(client::DEFAULT_SERVER_ADDRESS) + ".");
  text += engine::usageEntry(std::string(engine::PORT_OPTION) + " <port>",
                             "UDP port of the server. Default: " +
                                 std::to_string(network::DEFAULT_SERVER_PORT) +
                                 ".");
  text += engine::usageEntry(
      std::string(BOTS_OPTION) + " <count>",
      "Bots that join the server, from 1 to " +
          std::to_string(MAXIMUM_BOT_COUNT) +
          ". Default: " + std::to_string(DEFAULT_BOT_COUNT) + ".");
  text += engine::usageEntry(std::string(SECONDS_OPTION) + " <seconds>",
                             "How long the bots play, then leave. Default: " +
                                 std::to_string(DEFAULT_RUN_DURATION.count()) +
                                 ".");
  text += engine::usageEntry(
      std::string(SEED_OPTION) + " <seed>",
      "Seed the actions of the bots are drawn from. Default: " +
          std::to_string(DEFAULT_BOT_SEED) + ".");
  text += pilotUsage();
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and start no bot.");
  return text;
}

}  // namespace rtype::tools
