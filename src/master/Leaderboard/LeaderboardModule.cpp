#include "LeaderboardModule.hpp"
#include <functional>
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "LeaderboardController.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "ServersService.hpp"

namespace rtype::master {

std::string LeaderboardModule::name() const {
  return std::string{LEADERBOARD_MODULE_NAME};
}

std::vector<std::string> LeaderboardModule::dependencies() const {
  return {std::string{STORAGE_MODULE_NAME}, std::string{HTTP_MODULE_NAME},
          std::string{SERVERS_MODULE_NAME}};
}

void LeaderboardModule::install(engine::InstallContext& context) {
  LeaderboardController& controller = controller_.emplace(
      context.require<MasterDatabase>(), context.require<ServersService>());
  auto& server = context.require<MasterHttpServer>();
  server.add(
      network::KEEP_OWN_RESULT_ROUTE,
      std::bind_front(&LeaderboardController::keepOwnResult, &controller));
  server.add(
      network::READ_LEADERBOARD_ROUTE,
      std::bind_front(&LeaderboardController::readLeaderboard, &controller));
}

void LeaderboardModule::start() {}

void LeaderboardModule::stop() {}

}  // namespace rtype::master
