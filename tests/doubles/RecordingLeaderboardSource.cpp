#include "RecordingLeaderboardSource.hpp"
#include <utility>
#include <vector>
#include "LeaderboardPage.hpp"
#include "LeaderboardRequest.hpp"

using rtype::client::LeaderboardPage;
using rtype::network::LeaderboardRequest;

void RecordingLeaderboardSource::ask(const LeaderboardRequest& filter) {
  asked_.push_back(filter);
}

void RecordingLeaderboardSource::stopAsking() { ++stopCount_; }

const LeaderboardPage& RecordingLeaderboardSource::page() const {
  return page_;
}

void RecordingLeaderboardSource::setPage(LeaderboardPage page) {
  const auto nextRevision = page_.revision + 1;
  page_ = std::move(page);
  page_.revision = nextRevision;
}

const std::vector<LeaderboardRequest>& RecordingLeaderboardSource::asked()
    const {
  return asked_;
}

int RecordingLeaderboardSource::stopCount() const { return stopCount_; }
