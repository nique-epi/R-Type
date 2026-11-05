#include "ServerModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include "AssetFolder.hpp"
#include "EntitySheetConstants.hpp"
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
#include "ServerSupervisorLinkModule.hpp"
#include "StopRequest.hpp"
#include "SupervisorLink.hpp"
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
  const std::optional<SupervisorLink> link = launchOptions.supervisorLink();
  modules.push_back(std::make_unique<ServerGamesModule>(GamesSettings{
      .seed = std::nullopt,
      .wavesFile = engine::locateAssetFolder() / WAVE_FILE_IN_ASSETS,
      .entitySheetsFolder =
          engine::locateAssetFolder() / game::ENTITY_SHEET_FOLDER_IN_ASSETS,
      .gameCount = link.has_value() ? GAME_PROCESS_GAME_COUNT
                                    : DEFAULT_HOSTED_GAME_COUNT}));
  modules.push_back(std::make_unique<ServerLobbyModule>());
  modules.push_back(std::make_unique<ServerMetricsModule>(
      std::make_shared<engine::SystemClock>(), launchOptions.trafficCsvFile()));
  modules.push_back(std::make_unique<ServerMasterModule>());
  if (link.has_value()) {
    modules.push_back(
        std::make_unique<ServerSupervisorLinkModule>(*link, stopRequest));
  }
  return modules;
}

}  // namespace rtype::server
