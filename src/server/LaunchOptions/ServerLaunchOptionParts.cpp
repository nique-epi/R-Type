#include "ServerLaunchOptionParts.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "EndpointText.hpp"
#include "ForcedSetting.hpp"
#include "GameRuleConstants.hpp"
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

engine::LaunchOption difficultyOption(std::vector<std::string>& forcedLines) {
  return engine::settingOption(
      engine::ForcedSetting{.option = DIFFICULTY_OPTION,
                            .setting = DIFFICULTY_SETTING_NAME},
      forcedLines);
}

engine::LaunchOption livesOption(std::vector<std::string>& forcedLines) {
  return engine::settingOption(
      engine::ForcedSetting{.option = LIVES_OPTION,
                            .setting = LIVES_SETTING_NAME},
      forcedLines);
}

engine::LaunchOption friendlyFireOption(std::vector<std::string>& forcedLines) {
  return engine::settingOption(
      engine::ForcedSetting{.option = FRIENDLY_FIRE_OPTION,
                            .setting = FRIENDLY_FIRE_SETTING_NAME},
      forcedLines);
}

std::string ruleOptionEntries() {
  std::string text = engine::usageEntry(
      std::string(DIFFICULTY_OPTION) + " <name>",
      "Difficulty of the game, a table of the waves file. Default: " +
          std::string(identifiers::DEFAULT_DIFFICULTY) + ".");
  text += engine::usageEntry(
      std::string(LIVES_OPTION) + " <count>",
      "Lives each player starts with, from " +
          std::to_string(identifiers::LOWEST_LIVES) + " to " +
          std::to_string(identifiers::HIGHEST_LIVES) +
          ". Default: " + std::to_string(identifiers::DEFAULT_LIVES) + ".");
  text += engine::usageEntry(
      std::string(FRIENDLY_FIRE_OPTION) + " <0|1>",
      "1 when a missile of a player costs another player a life. Default: " +
          std::to_string(FRIENDLY_FIRE_OFF) + ".");
  return text;
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
