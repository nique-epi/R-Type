#include "SupervisorModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "BoostProcessLauncher.hpp"
#include "BuiltModuleConstants.hpp"
#include "EngineException.hpp"
#include "ExecutablePath.hpp"
#include "GameLaunchSettings.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "NetworkConstants.hpp"
#include "ServerAdminConsoleModule.hpp"
#include "ServerContentModule.hpp"
#include "ServerInformationModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerNetworkModule.hpp"
#include "ServerSupervisedGamesModule.hpp"
#include "ServerSupervisorLobbyModule.hpp"
#include "StandardInput.hpp"
#include "StandardOutput.hpp"
#include "StopRequest.hpp"
#include "TimeConstants.hpp"
#if RTYPE_MODULE_MASTER_LINK
#include "ServerMasterModule.hpp"
#endif

namespace rtype::server {

namespace {

/**
 * @returns What every game process is started with: this executable, the
 * folder next to it, @p gameLines, the lines of its file of commands, and what
 * @p launchOptions hand the games: their logging options, the network
 * simulated and the traffic file.
 * @throws engine::UnknownExecutablePathException when the system does not tell
 * where the executable is.
 */
GameLaunchSettings gameLaunchSettings(const LaunchOptions& launchOptions,
                                      std::vector<std::string> gameLines) {
  const std::optional<std::filesystem::path> program = engine::executablePath();
  if (!program.has_value()) {
    throw engine::UnknownExecutablePathException();
  }
  return GameLaunchSettings{.program = *program,
                            .folder = engine::executableFolder(),
                            .gameLines = std::move(gameLines),
                            .forwardedArguments = launchOptions.gameArguments(),
                            .trafficCsvFile = launchOptions.trafficCsvFile()};
}

}  // namespace

std::vector<std::unique_ptr<engine::IModule>> supervisorModules(
    const LaunchOptions& launchOptions, StopRequest& stopRequest,
    std::vector<std::string> gameLines) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(
      std::make_unique<ServerLoggerModule>(launchOptions.logOptions()));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      network::DEFAULT_SERVER_PORT, stopRequest,
      launchOptions.networkConditions()));
  modules.push_back(std::make_unique<ServerContentModule>());
  modules.push_back(std::make_unique<ServerSupervisedGamesModule>(
      gameLaunchSettings(launchOptions, std::move(gameLines)),
      std::make_unique<BoostProcessLauncher>(),
      [](engine::Duration pause) { std::this_thread::sleep_for(pause); }));
  modules.push_back(std::make_unique<ServerSupervisorLobbyModule>());
  modules.push_back(std::make_unique<ServerInformationModule>());
#if RTYPE_MODULE_MASTER_LINK
  modules.push_back(std::make_unique<ServerMasterModule>());
#endif
  modules.push_back(std::make_unique<ServerAdminConsoleModule>(
      std::make_unique<StandardInput>(), std::make_unique<StandardOutput>()));
  return modules;
}

}  // namespace rtype::server
