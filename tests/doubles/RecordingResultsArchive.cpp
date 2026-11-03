#include "RecordingResultsArchive.hpp"
#include <cstddef>
#include <utility>
#include <vector>
#include "GameResult.hpp"
#include "LeaderboardAsk.hpp"
#include "LeaderboardReply.hpp"

void RecordingResultsArchive::record(rtype::server::GameResult result) {
  results_.push_back(std::move(result));
}

void RecordingResultsArchive::ask(rtype::server::LeaderboardAsk ask) {
  asks_.push_back(std::move(ask));
}

void RecordingResultsArchive::answerEveryAsk(
    const std::vector<std::byte>& payload) {
  for (const rtype::server::LeaderboardAsk& ask : asks_) {
    ask.replies->push(
        rtype::server::LeaderboardReply{.recipient = ask.asker,
                                        .sessionToken = ask.sessionToken,
                                        .payload = payload});
  }
  asks_.clear();
}

const std::vector<rtype::server::GameResult>& RecordingResultsArchive::results()
    const {
  return results_;
}

const std::vector<rtype::server::LeaderboardAsk>&
RecordingResultsArchive::asks() const {
  return asks_;
}
