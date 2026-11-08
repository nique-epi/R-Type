#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardQuery.hpp"
#include "GlobalLeaderboardReading.hpp"
#include "IMasterLeaderboardReads.hpp"

/**
 * @brief Leaderboard reads double: it writes the asks down and numbers them as
 * MasterLink does, and answers only when the test says so, to the ask the test
 * names, so an answer to an older ask can come after a newer ask.
 */
class RecordingMasterLeaderboardReads final
    : public rtype::client::IMasterLeaderboardReads {
 public:
  std::uint64_t askLeaderboard(
      const rtype::network::GlobalLeaderboardQuery& query) override;
  [[nodiscard]] std::optional<rtype::client::GlobalLeaderboardReading>
  leaderboard() const override;

  /** @brief Answers the ask numbered @p ask with @p leaderboard; nothing means
   * the master did not answer. */
  void answer(std::uint64_t ask,
              std::optional<rtype::network::GlobalLeaderboard> leaderboard);

  /** @returns The query of each ask, in order. */
  [[nodiscard]] const std::vector<rtype::network::GlobalLeaderboardQuery>&
  queries() const;

 private:
  std::vector<rtype::network::GlobalLeaderboardQuery> queries_;
  std::optional<rtype::client::GlobalLeaderboardReading> reading_;
};
