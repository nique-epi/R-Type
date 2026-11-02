#include "ServerModules.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "AssetFolder.hpp"
#include "ExecutablePath.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesModule.hpp"
#include "ServerLobbyModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerMetricsModule.hpp"
#include "ServerNetworkModule.hpp"
#include "ServerStorageModule.hpp"
#include "StopRequest.hpp"
#include "SystemClock.hpp"

namespace rtype::server {

namespace {

/** @returns The file DATABASE_OPTION names, or the one of the port of the
 * server, next to its executable. */
std::filesystem::path databaseFileOf(const LaunchOptions& options) {
  return options.databaseFile().value_or(engine::executableFolder() /
                                         (std::string{DATABASE_FILE_PREFIX} +
                                          std::to_string(options.port()) +
                                          std::string{DATABASE_FILE_SUFFIX}));
}

}  // namespace

std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, StopRequest& stopRequest) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(logOptions));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      launchOptions.port(), stopRequest, launchOptions.networkConditions()));
  modules.push_back(
      std::make_unique<ServerStorageModule>(databaseFileOf(launchOptions)));
  modules.push_back(std::make_unique<ServerGamesModule>(GamesSettings{
      .seed = launchOptions.seed(),
      .wavesFile = engine::locateAssetFolder() / WAVE_FILE_IN_ASSETS,
      .statesPerSecond = launchOptions.stateRate(),
      .inputDelayTicks = launchOptions.inputDelay()}));
  modules.push_back(std::make_unique<ServerLobbyModule>());
  modules.push_back(std::make_unique<ServerMetricsModule>(
      std::make_shared<engine::SystemClock>()));
  return modules;
}

}  // namespace rtype::server
