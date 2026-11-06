#include "SupervisorModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "BoostProcessLauncher.hpp"
#include "EngineException.hpp"
#include "ExecutablePath.hpp"
#include "GameLaunchSettings.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "NetworkConstants.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerNetworkModule.hpp"
#include "ServerSupervisedGamesModule.hpp"
#include "ServerSupervisorLobbyModule.hpp"
#include "StopRequest.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns What every game process is started with: this executable, the
 * folder next to it, and @p gameLines, the lines of its file of commands.
 * @throws engine::UnknownExecutablePathException when the system does not tell
 * where the executable is.
 */
GameLaunchSettings gameLaunchSettings(
    const std::vector<std::string>& gameLines) {
  const std::optional<std::filesystem::path> program = engine::executablePath();
  if (!program.has_value()) {
    throw engine::UnknownExecutablePathException();
  }
  return GameLaunchSettings{.program = *program,
                            .folder = engine::executableFolder(),
                            .gameLines = &gameLines};
}

}  // namespace

std::vector<std::unique_ptr<engine::IModule>> supervisorModules(
    const LaunchOptions& launchOptions, StopRequest& stopRequest,
    const std::vector<std::string>& gameLines) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(
      std::make_unique<ServerLoggerModule>(launchOptions.logOptions()));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      network::DEFAULT_SERVER_PORT, stopRequest,
      launchOptions.networkConditions()));
  modules.push_back(std::make_unique<ServerSupervisedGamesModule>(
      gameLaunchSettings(gameLines), std::make_unique<BoostProcessLauncher>(),
      [](engine::Duration pause) { std::this_thread::sleep_for(pause); }));
  modules.push_back(std::make_unique<ServerSupervisorLobbyModule>());
  return modules;
}

}  // namespace rtype::server
