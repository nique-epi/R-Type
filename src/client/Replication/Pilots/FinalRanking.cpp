#include "FinalRanking.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <string>
#include <vector>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "PilotStatus.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p first ranks before @p second: a higher score, or the
 * same score and a lower number. */
bool ranksBefore(const FinalPilot& first, const FinalPilot& second) {
  if (first.score != second.score) {
    return first.score > second.score;
  }
  return first.playerNumber < second.playerNumber;
}

}  // namespace

std::vector<FinalPilot> finalPilotsOf(std::span<const PilotStatus> pilots,
                                      const network::GameEnded& ended) {
  std::vector<FinalPilot> finalPilots;
  finalPilots.reserve(ended.players.size());
  for (const network::PlayerScore& player : ended.players) {
    const auto status = std::ranges::find(pilots, player.playerNumber,
                                          &PilotStatus::playerNumber);
    const bool listed = status != pilots.end();
    finalPilots.push_back(
        FinalPilot{.playerNumber = player.playerNumber,
                   .nickname = listed ? status->nickname : std::string{},
                   .score = player.score,
                   .isLocalPlayer = listed && status->isLocalPlayer});
  }
  return finalPilots;
}

std::vector<FinalPilot> rankedPilots(std::vector<FinalPilot> pilots) {
  std::ranges::sort(pilots, ranksBefore);
  for (std::size_t place = 0; place < pilots.size(); ++place) {
    const bool tied =
        place > 0 && pilots[place].score == pilots[place - 1].score;
    pilots[place].rank =
        tied ? pilots[place - 1].rank : static_cast<int>(place) + 1;
  }
  return pilots;
}

std::uint32_t teamScoreOf(std::span<const FinalPilot> pilots) {
  std::uint64_t total = 0;
  for (const FinalPilot& pilot : pilots) {
    total += pilot.score;
  }
  return static_cast<std::uint32_t>(std::min<std::uint64_t>(
      total, std::numeric_limits<std::uint32_t>::max()));
}

}  // namespace rtype::client
