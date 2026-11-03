#include "ResultsArchiveFixture.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "LeaderboardAsk.hpp"
#include "LeaderboardReply.hpp"
#include "LeaderboardRequest.hpp"
#include "ResultsDatabaseTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ServerTestConstants.hpp"

namespace rtype::server::testing {

LeaderboardAsk ResultsArchiveFixture::askFor(
    const network::LeaderboardRequest& board) {
  return LeaderboardAsk{.request = board,
                        .nickname = std::string{ACE},
                        .asker = playerEndpoint(FIRST_ASKER),
                        .sessionToken = SESSION_TOKEN,
                        .replies = &replies_};
}

std::optional<LeaderboardReply> ResultsArchiveFixture::nextReply() {
  const auto deadline = std::chrono::steady_clock::now() + THREAD_TIMEOUT;
  std::vector<LeaderboardReply> received;
  while (received.empty() && std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
    replies_.drainInto(received);
  }
  if (received.empty()) {
    return std::nullopt;
  }
  return received.front();
}

}  // namespace rtype::server::testing
