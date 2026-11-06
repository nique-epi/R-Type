#include "HeartbeatJobFixture.hpp"
#include <chrono>
#include <cstdint>
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
#include "RunningMaster.hpp"
#include "ServerIdentity.hpp"

namespace rtype::server::testing {

namespace {

/** @returns Whether @p condition holds within NOTICE_DEADLINE, asked every
 * POLL_STEP. */
template <typename Condition>
bool holdsSoon(Condition condition) {
  const auto deadline = std::chrono::steady_clock::now() + NOTICE_DEADLINE;
  while (std::chrono::steady_clock::now() < deadline) {
    if (condition()) {
      return true;
    }
    std::this_thread::sleep_for(POLL_STEP);
  }
  return false;
}

}  // namespace

HeartbeatJobFixture::HeartbeatJobFixture()
    : created_(master_.createServer(LINKED_NAME)) {}

void HeartbeatJobFixture::startJob(HeartbeatTimes times) {
  job_.emplace(HeartbeatJobParts{
      .access =
          network::MasterAccess{.url = master_.url(), .key = created_.key},
      .identity = ServerIdentity{.name = std::string{LINKED_NAME},
                                 .udpPort = LINKED_PORT,
                                 .maximumGames = LINKED_GAMES,
                                 .probeSecret = LINKED_PROBE_SECRET},
      .games = games_,
      .probeTokens = probeTokens_,
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

bool HeartbeatJobFixture::heartbeatSoonNames(std::string_view text) const {
  return holdsSoon([this, text] {
    return master_.lastHeartbeatOf(created_.id)
               .value_or(std::string{})
               .find(text) != std::string::npos;
  });
}

RunningMaster& HeartbeatJobFixture::master() { return master_; }

void HeartbeatJobFixture::publishProbeToken(std::uint64_t token) {
  probeTokens_.publish(token);
}

bool HeartbeatJobFixture::heartbeatSoonLacks(std::string_view text) const {
  return holdsSoon([this, text] {
    const std::optional<std::string> heartbeat =
        master_.lastHeartbeatOf(created_.id);
    return heartbeat.has_value() && heartbeat->find(text) == std::string::npos;
  });
}

}  // namespace rtype::server::testing
