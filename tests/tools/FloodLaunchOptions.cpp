#include "FloodLaunchOptions.hpp"
#include <array>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include "EndpointText.hpp"
#include "FloodConstants.hpp"
#include "FloodLaunch.hpp"
#include "FloodMode.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "NetworkConstants.hpp"

namespace rtype::tools {

namespace {

/**
 * @returns What reading the option of @p mode does: @p launch floods in that
 * mode, on the port given as its value.
 */
std::function<void(std::string_view)> recordMode(FloodLaunch& launch,
                                                 FloodMode mode) {
  return [&launch, mode](std::string_view value) {
    launch.port = network::portFromText(value);
    launch.mode = mode;
  };
}

}  // namespace

FloodLaunch readFloodLaunch(std::span<const std::string_view> arguments) {
  FloodLaunch launch;
  const std::array options{
      engine::LaunchOption{.name = SERVER_OPTION,
                           .takesValue = true,
                           .record = recordMode(launch, FloodMode::Server)},
      engine::LaunchOption{.name = CLIENT_OPTION,
                           .takesValue = true,
                           .record = recordMode(launch, FloodMode::Client)},
      engine::LaunchOption{
          .name = engine::HELP_OPTION,
          .takesValue = false,
          .record =
              [&launch](std::string_view) { launch.helpRequested = true; }},
  };
  engine::readLaunchArguments(arguments, options);
  return launch;
}

std::string floodUsage() {
  const std::string loopback{network::LOOPBACK_IPV4_ADDRESS};
  const std::string seconds = std::to_string(FLOOD_DURATION.count());
  std::string text = engine::usageHeader(TOOL_PROGRAM_NAME);
  text += engine::usageEntry(
      std::string{SERVER_OPTION} + " <port>",
      "Join the server listening on that port of " + loopback +
          " as a player, send it malformed datagrams for " + seconds +
          " s, then check that it still answers a connection request.");
  text += engine::usageEntry(
      std::string{CLIENT_OPTION} + " <port>",
      "Wait on that port of " + loopback +
          " for a client, accept it, then send it malformed datagrams for " +
          seconds + " s.");
  text += engine::usageEntry(engine::HELP_OPTION,
                             "Print this help and send nothing.");
  return text;
}

}  // namespace rtype::tools
