#include "ServerModules.hpp"
#include <filesystem>
#include <memory>
#include <vector>
#include "AssetFolder.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesModule.hpp"
#include "ServerLobbyModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerNetworkModule.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, StopRequest& stopRequest) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(logOptions));
  modules.push_back(
      std::make_unique<ServerNetworkModule>(launchOptions.port(), stopRequest));
  modules.push_back(std::make_unique<ServerGamesModule>(GamesSettings{
      .seed = launchOptions.seed(),
      .wavesFile = engine::locateAssetFolder() / WAVE_FILE_IN_ASSETS,
      .statesPerSecond = launchOptions.stateRate()}));
  modules.push_back(std::make_unique<ServerLobbyModule>());
  return modules;
}

}  // namespace rtype::server
