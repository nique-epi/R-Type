#pragma once

#include "GameResult.hpp"
#include "LeaderboardAsk.hpp"

namespace rtype::server {

/**
 * @brief What the games ask of the storage of the server. Each call returns at
 * once, whatever the disk does, and is safe from any thread.
 */
class IResultsArchive {
 public:
  virtual ~IResultsArchive() = default;

  /** @brief Keeps @p result, saved shortly after. */
  virtual void record(GameResult result) = 0;

  /** @brief Reads the leaderboard @p ask names and pushes the answer to the
   * queue it carries, shortly after. */
  virtual void ask(LeaderboardAsk ask) = 0;
};

}  // namespace rtype::server
