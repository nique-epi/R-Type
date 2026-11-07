#include "GameProcessModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "BuiltModuleConstants.hpp"
#include "ContentFolders.hpp"
#include "EnsureLines.hpp"
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

/** @returns What the games module of a game process is built from: the
 * waves file of the last of @p contentFolders that holds one, and the entity
 * sheets of all of them. */
GamesSettings gameProcessGamesSettings(
    std::vector<std::filesystem::path> contentFolders) {
  std::filesystem::path wavesFile = engine::topmostContentFile(
      contentFolders, std::filesystem::path{WAVE_FILE_IN_ASSETS});
  return GamesSettings{.seed = std::nullopt,
                       .wavesFile = std::move(wavesFile),
                       .contentFolders = std::move(contentFolders)};
}

}  // namespace

std::vector<std::unique_ptr<engine::IModule>> gameProcessModules(
    const GameLaunchOptions& launchOptions, StopRequest& stopRequest,
    std::span<const std::string> handedOverLines) {
  const std::vector<std::filesystem::path> contentFolders =
      engine::contentFoldersOf(engine::locateResourcePlaces(),
                               engine::ensuredResources(handedOverLines));
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(
      std::make_unique<ServerLoggerModule>(launchOptions.logOptions()));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      network::DEFAULT_SERVER_PORT, stopRequest,
      launchOptions.networkConditions()));
  modules.push_back(
      std::make_unique<ServerStorageModule>(engine::executableFolder()));
  modules.push_back(std::make_unique<ServerGamesModule>(
      gameProcessGamesSettings(contentFolders)));
  modules.push_back(std::make_unique<ServerLobbyModule>(
      launchOptions.supervisorLink().port, contentFolders));
#if RTYPE_MODULE_METRICS
  modules.push_back(std::make_unique<ServerMetricsModule>(
      std::make_shared<engine::SystemClock>(), launchOptions.trafficCsvFile()));
#endif
  modules.push_back(std::make_unique<ServerSupervisorLinkModule>(
      launchOptions.supervisorLink(), stopRequest));
  return modules;
}

}  // namespace rtype::server
