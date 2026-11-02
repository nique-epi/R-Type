#include "ServerModules.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "LogLaunchOptions.hpp"
#include "NetworkConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesModule.hpp"
#include "ServerLobbyModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerNetworkModule.hpp"
#include "StopRequest.hpp"

namespace rtype::server {

std::vector<std::unique_ptr<engine::IModule>> serverModules(
    const logging::LogLaunchOptions& logOptions, StopRequest& stopRequest) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerLoggerModule>(logOptions));
  modules.push_back(std::make_unique<ServerNetworkModule>(
      network::DEFAULT_SERVER_PORT, stopRequest));
  modules.push_back(std::make_unique<ServerGamesModule>(
      GamesSettings{.seed = std::nullopt,
                    .wavesFile = std::filesystem::path{WAVE_FILE_PATH}}));
  modules.push_back(std::make_unique<ServerLobbyModule>());
  return modules;
}

}  // namespace rtype::server
