#include "GameCommand.hpp"
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include "GameLaunch.hpp"
#include "GameLaunchSettings.hpp"
#include "LaunchArgumentsConstants.hpp"
#include "LoggingConstants.hpp"
#include "ProcessCommand.hpp"
#include "ServerConstants.hpp"
#include "SupervisedGamesConstants.hpp"

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

/** @returns The leaderboard file of the supervisor on @p supervisorPort, in
 * @p folder. */
std::string databaseFileOf(const std::filesystem::path& folder,
                           std::uint16_t supervisorPort) {
  return (folder /
          (std::string{DATABASE_FILE_PREFIX} + std::to_string(supervisorPort) +
           std::string{DATABASE_FILE_SUFFIX}))
      .string();
}

}  // namespace

ProcessCommand gameCommand(const GameLaunchSettings& settings,
                           const GameLaunch& launch) {
  ProcessCommand command{.program = settings.program, .arguments = {}};
  addOption(command, MODE_OPTION, std::string{GAME_MODE_NAME});
  addOption(command, engine::PORT_OPTION, std::to_string(launch.port));
  addOption(command, SUPERVISOR_OPTION, std::to_string(launch.supervisorPort));
  addOption(command, LINK_TOKEN_OPTION, std::to_string(launch.linkToken));
  addOption(command, engine::DATABASE_OPTION,
            databaseFileOf(settings.folder, launch.supervisorPort));
  addOption(command, logging::FILE_OPTION,
            logFileOf(settings.folder, launch.number));
  return command;
}

}  // namespace rtype::server
