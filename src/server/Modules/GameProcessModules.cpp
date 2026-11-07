#include "GameProcessModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include "AssetFolder.hpp"
#include "BuiltModuleConstants.hpp"
#include "EntitySheetConstants.hpp"
#include "ExecutablePath.hpp"
#include "GameLaunchOptions.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesModule.hpp"
#include "ServerLobbyModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerNetworkModule.hpp"
#include "ServerStorageModule.hpp"
#include "ServerSupervisorLinkModule.hpp"
#include "StopRequest.hpp"
#if RTYPE_MODULE_METRICS
#include "ServerMetricsModule.hpp"
#include "SystemClock.hpp"
#endif

namespace rtype::server {

namespace {

/** @returns What the games module of a game process is built from: the waves
 * and the entity sheets of the assets folder. */
GamesSettings gameProcessGamesSettings() {
  return GamesSettings{
      .seed = std::nullopt,
      .wavesFile = engine::locateAssetFolder() / WAVE_FILE_IN_ASSETS,
      .entitySheetsFolder =
          engine::locateAssetFolder() / game::ENTITY_SHEET_FOLDER_IN_ASSETS};
}

}  // namespace

std::vector<std::unique_ptr<engine::IModule>> gameProcessModules(
    const GameLaunchOptions& launchOptions, StopRequest& stopRequest) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(
      std::make_unique<ServerLoggerModule>(launchOptions.logOptions()));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      network::DEFAULT_SERVER_PORT, stopRequest,
      launchOptions.networkConditions()));
  modules.push_back(
      std::make_unique<ServerStorageModule>(engine::executableFolder()));
  modules.push_back(
      std::make_unique<ServerGamesModule>(gameProcessGamesSettings()));
  modules.push_back(std::make_unique<ServerLobbyModule>());
#if RTYPE_MODULE_METRICS
  modules.push_back(std::make_unique<ServerMetricsModule>(
      std::make_shared<engine::SystemClock>(), launchOptions.trafficCsvFile()));
#endif
  modules.push_back(std::make_unique<ServerSupervisorLinkModule>(
      launchOptions.supervisorLink(), stopRequest));
  return modules;
}

}  // namespace rtype::server
