#include "ServerModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include "AssetFolder.hpp"
#include "ExecutablePath.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesModule.hpp"
#include "ServerLobbyModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerMasterModule.hpp"
#include "ServerMetricsModule.hpp"
#include "ServerNetworkModule.hpp"
#include "ServerStorageModule.hpp"
#include "StopRequest.hpp"
#include "SystemClock.hpp"

namespace rtype::server {

std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions,
    const LaunchOptions& launchOptions, StopRequest& stopRequest) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(logOptions));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      network::DEFAULT_SERVER_PORT, stopRequest,
      launchOptions.networkConditions()));
  modules.push_back(
      std::make_unique<ServerStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<ServerGamesModule>(GamesSettings{
      .seed = std::nullopt,
      .wavesFile = engine::locateAssetFolder() / WAVE_FILE_IN_ASSETS}));
  modules.push_back(std::make_unique<ServerLobbyModule>());
  modules.push_back(std::make_unique<ServerMetricsModule>(
      std::make_shared<engine::SystemClock>(), launchOptions.trafficCsvFile()));
  modules.push_back(std::make_unique<ServerMasterModule>());
  return modules;
}

}  // namespace rtype::server
