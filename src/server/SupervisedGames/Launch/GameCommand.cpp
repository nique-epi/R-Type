#include "GameCommand.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include "GameConfigurationFile.hpp"
#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "GameRules.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ProcessCommand.hpp"
#include "ServerConstants.hpp"
#include "ServerDatabaseFile.hpp"
#include "SupervisedGamesConstants.hpp"
#include "TextBeforeExtension.hpp"

namespace rtype::server {

namespace {

/** @brief Adds @p option and its @p value to the arguments of @p command. */
void addOption(ProcessCommand& command, std::string_view option,
               const std::string& value) {
  command.arguments.emplace_back(option);
  command.arguments.push_back(value);
}

/** @returns The journal file of the game @p number, in @p folder. */
std::string logFileOf(const std::filesystem::path& folder,
                      std::uint32_t number) {
  return (folder / (std::string{GAME_LOG_FILE_PREFIX} + std::to_string(number) +
                    std::string{GAME_LOG_FILE_SUFFIX}))
      .string();
}

/** @returns The traffic file of the game @p number, from @p given, the file
 * given to the supervisor: its name, GAME_TRAFFIC_FILE_INFIX and the number,
 * then its extension, in its folder. */
std::string trafficFileOf(const std::filesystem::path& given,
                          std::uint32_t number) {
  return engine::withTextBeforeExtension(
             given,
             std::string{GAME_TRAFFIC_FILE_INFIX} + std::to_string(number))
      .string();
}

/** @brief Adds the options of @p rules to the arguments of @p command. */
void addRuleOptions(ProcessCommand& command, const network::GameRules& rules) {
  addOption(command, DIFFICULTY_OPTION, rules.difficulty);
  addOption(command, LIVES_OPTION, std::to_string(rules.lives));
  addOption(command, FRIENDLY_FIRE_OPTION,
            std::to_string(rules.friendlyFire ? FRIENDLY_FIRE_ON
                                              : FRIENDLY_FIRE_OFF));
}

}  // namespace

ProcessCommand gameCommand(const GameLaunchSettings& settings,
                           const GameLaunch& launch,
                           std::string_view databaseSetting) {
  ProcessCommand command{.program = settings.program, .arguments = {}};
  addOption(command, MODE_OPTION, std::string{GAME_MODE_NAME});
  addOption(command, engine::PORT_OPTION, std::to_string(launch.port));
  addOption(command, SUPERVISOR_OPTION, std::to_string(launch.supervisorPort));
  addOption(command, LINK_TOKEN_OPTION, std::to_string(launch.linkToken));
  addOption(
      command, engine::DATABASE_OPTION,
      databaseFileOf(databaseSetting, settings.folder, launch.supervisorPort)
          .string());
  addOption(command, logging::FILE_OPTION,
            logFileOf(settings.folder, launch.number));
  addOption(
      command, CONFIGURATION_OPTION,
      gameConfigurationFileOf(settings.folder, launch.supervisorPort).string());
  addRuleOptions(command, launch.rules);
  command.arguments.insert(command.arguments.end(),
                           settings.forwardedArguments.begin(),
                           settings.forwardedArguments.end());
  if (settings.trafficCsvFile.has_value()) {
    addOption(command, network::TRAFFIC_CSV_OPTION,
              trafficFileOf(*settings.trafficCsvFile, launch.number));
  }
  return command;
}

}  // namespace rtype::server
