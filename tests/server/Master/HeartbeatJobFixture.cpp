#include "HeartbeatJobFixture.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include "HeartbeatJobParts.hpp"
#include "HeartbeatJobTestConstants.hpp"
#include "HeartbeatTimes.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "MasterAccess.hpp"
#include "MasterClient.hpp"
#include "RunningMaster.hpp"
#include "ServerCreation.hpp"
#include "ServerIdentity.hpp"

namespace rtype::server::testing {

HeartbeatJobFixture::HeartbeatJobFixture()
    : created_(network::MasterClient{
          network::MasterAccess{.url = master_.url(), .key = {}}}
                   .createServer(network::ServerCreation{
                       .name = std::string{LINKED_NAME}})) {}

void HeartbeatJobFixture::startJob(HeartbeatTimes times) {
  job_.emplace(HeartbeatJobParts{
      .access =
          network::MasterAccess{.url = master_.url(), .key = created_.key},
      .identity = ServerIdentity{.name = std::string{LINKED_NAME},
                                 .udpPort = LINKED_PORT,
                                 .maximumGames = LINKED_GAMES},
      .games = games_,
      .measures = nullptr,
      .times = times});
}

void HeartbeatJobFixture::publishGameOf(std::string_view pilot) {
  games_.publish(std::vector<network::HostedGame>{
      network::HostedGame{.number = LINKED_GAME_NUMBER,
                          .name = std::string{LINKED_GAME_NAME},
                          .state = network::HostedGameState::Waiting,
                          .difficulty = std::string{LINKED_DIFFICULTY},
                          .players = {std::string{pilot}}}});
}

bool HeartbeatJobFixture::heartbeatSoonNames(std::string_view pilot) const {
  const auto deadline = std::chrono::steady_clock::now() + NOTICE_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    const std::optional<std::string> heartbeat =
        master_.lastHeartbeatOf(created_.id);
    if (heartbeat.value_or(std::string{}).find(pilot) != std::string::npos) {
      return true;
    }
    std::this_thread::sleep_for(POLL_STEP);
  }
  return false;
}

RunningMaster& HeartbeatJobFixture::master() { return master_; }

}  // namespace rtype::server::testing
