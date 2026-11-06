#include "ServerLaunchOptionParts.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "EndpointText.hpp"
#include "ForcedSetting.hpp"
#include "LaunchArguments.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LaunchOption.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "SettingLine.hpp"
#include "SettingOptions.hpp"

namespace rtype::server {

engine::LaunchOption portOption(std::vector<std::string>& forcedLines) {
  return engine::LaunchOption{
      .name = engine::PORT_OPTION,
      .takesValue = true,
      .record = [&forcedLines](std::string_view port) {
        forcedLines.push_back(engine::settingLine(
            PORT_SETTING_NAME, std::to_string(network::portFromText(port))));
      }};
}

engine::LaunchOption databaseOption(std::vector<std::string>& forcedLines) {
  return engine::settingOption(
      engine::ForcedSetting{.option = engine::DATABASE_OPTION,
                            .setting = DATABASE_SETTING_NAME},
      forcedLines);
}

std::string portOptionEntry() {
  return engine::usageEntry(
      std::string(engine::PORT_OPTION) + " <port>",
      "UDP port to listen on, from " + std::to_string(network::LOWEST_PORT) +
          " to " + std::to_string(network::HIGHEST_PORT) +
          ". Default: " + std::to_string(network::DEFAULT_SERVER_PORT) + ".");
}

std::string databaseOptionEntry() {
  return engine::usageEntry(
      std::string(engine::DATABASE_OPTION) + " <file>",
      "File the leaderboard of this server is kept in, on a local disk. "
      "Default: " +
          std::string(DATABASE_FILE_PREFIX) + "<port>" +
          std::string(DATABASE_FILE_SUFFIX) + " next to the executable.");
}

std::string helpOptionEntry() {
  return engine::usageEntry(engine::HELP_OPTION,
                            "Print this help and start nothing.");
}

}  // namespace rtype::server
