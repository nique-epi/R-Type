#include "ResultsDatabaseFixture.hpp"
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "GameResult.hpp"
#include "LeaderboardRequest.hpp"
#include "ResultsDatabase.hpp"
#include "ResultsDatabaseTestConstants.hpp"

namespace rtype::server::testing {

PlayerResult playerNamed(std::string_view nickname, std::uint32_t score) {
  return PlayerResult{
      .nickname = std::string{nickname}, .score = score, .deaths = 0};
}

GameResult normalGame(std::vector<PlayerResult> players, std::int64_t endedAt) {
  return GameResult{.difficulty = std::string{NORMAL},
                    .playerCount = TWO_PLAYERS,
                    .endedAt = endedAt,
                    .players = std::move(players)};
}

network::LeaderboardRequest normalBoard() {
  return network::LeaderboardRequest{.playerCount = TWO_PLAYERS,
                                     .difficulty = std::string{NORMAL}};
}

void saveThenDieAtOnce(const std::filesystem::path& file,
                       const GameResult& result) {
  ResultsDatabase database{file};
  database.save(result);
  std::_Exit(DIED_AT_ONCE_EXIT_CODE);
}

std::filesystem::path ResultsDatabaseFixture::file() const {
  return folder_.path() / RESULTS_FILE_NAME;
}

}  // namespace rtype::server::testing
