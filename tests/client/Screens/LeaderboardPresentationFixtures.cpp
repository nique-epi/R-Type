#include "LeaderboardPresentationFixtures.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardPresentationTestConstants.hpp"

using rtype::client::LeaderboardPage;
using rtype::client::LeaderboardPhase;
using rtype::client::testing::ENTRY_SCORE;
using rtype::client::testing::OCTOBER_18_2026_SECONDS;
using rtype::network::LeaderboardRow;

LeaderboardRow entryOf(std::uint32_t rank, const std::string& name) {
  return LeaderboardRow{.rank = rank,
                        .score = ENTRY_SCORE,
                        .playedAt = OCTOBER_18_2026_SECONDS,
                        .nickname = name};
}

LeaderboardPage answeredPage(std::vector<LeaderboardRow> rows,
                             std::optional<LeaderboardRow> own) {
  return LeaderboardPage{.phase = LeaderboardPhase::Answered,
                         .rows = std::move(rows),
                         .ownRow = std::move(own)};
}
