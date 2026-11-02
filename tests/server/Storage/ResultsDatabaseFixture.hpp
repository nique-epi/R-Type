#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <filesystem>
#include <string_view>
#include <vector>
#include "GameResult.hpp"
#include "LeaderboardRequest.hpp"
#include "TemporaryFolder.hpp"

namespace rtype::server::testing {

/** @returns A row for @p nickname with @p score and no death. */
[[nodiscard]] PlayerResult playerNamed(std::string_view nickname,
                                       std::uint32_t score);

/** @returns A game of two players at the normal difficulty, ended at
 * @p endedAt, with @p players. */
[[nodiscard]] GameResult normalGame(std::vector<PlayerResult> players,
                                    std::int64_t endedAt);

/** @returns A request for the leaderboard of normalGame(). */
[[nodiscard]] network::LeaderboardRequest normalBoard();

/** @brief Saves @p result in @p file, then ends the process at once: no
 * destructor runs, so nothing is closed, as a process killed. */
[[noreturn]] void saveThenDieAtOnce(const std::filesystem::path& file,
                                    const GameResult& result);

/** @brief An empty folder and the database of a server in it. */
class ResultsDatabaseFixture : public ::testing::Test {
 protected:
  [[nodiscard]] std::filesystem::path file() const;

 private:
  TemporaryFolder folder_;
};

}  // namespace rtype::server::testing
