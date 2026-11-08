#include "HeartbeatJobFixture.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include "GameResult.hpp"
#include "HeartbeatJobParts.hpp"
#include "HeartbeatJobTestConstants.hpp"
#include "HeartbeatTimes.hpp"
#include "HostedGame.hpp"
#include "HostedGameState.hpp"
#include "MasterAccess.hpp"
#include "ResultsFileFixtures.hpp"
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
      .difficulties = {std::string{LINKED_DIFFICULTY}},
      .probeTokens = probeTokens_,
      .measures = nullptr,
      .times = times,
      .resultsFile = folder_.path() / LINKED_RESULTS_FILE});
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

std::string HeartbeatJobFixture::saveResult(const GameResult& result) const {
  return saveResultIn(folder_.path() / LINKED_RESULTS_FILE, result);
}

bool HeartbeatJobFixture::masterSoonKeeps(std::string_view gameId,
                                          int players) const {
  return holdsSoon([this, gameId, players] {
    return master_.keptPlayersOf(gameId) == players;
  });
}

MasterAnswers HeartbeatJobFixture::soonAnswered(std::string_view gameId) const {
  static_cast<void>(holdsSoon([this, gameId] {
    const MasterAnswers answers = answersOf(gameId);
    return answers.sent || answers.refused;
  }));
  return answersOf(gameId);
}

MasterAnswers HeartbeatJobFixture::answersOf(std::string_view gameId) const {
  return answersIn(folder_.path() / LINKED_RESULTS_FILE, gameId);
}

bool HeartbeatJobFixture::heartbeatSoonLacks(std::string_view text) const {
  return holdsSoon([this, text] {
    const std::optional<std::string> heartbeat =
        master_.lastHeartbeatOf(created_.id);
    return heartbeat.has_value() && heartbeat->find(text) == std::string::npos;
  });
}

}  // namespace rtype::server::testing
