#pragma once

#include <cstdint>
#include <map>
#include <string>
#include "EnemyKilled.hpp"

namespace rtype::server {

/**
 * @brief The rows of the statistics file not written yet, and the points of
 * each game in progress.
 *
 * Each game that starts takes the next number, from 1, whatever bus it runs
 * on, so the rows of two games never mix. Not thread-safe: use it from the
 * simulation thread only.
 */
class StatisticsSheet {
 public:
  /** @returns The number of a game that starts: one more than the last. */
  [[nodiscard]] std::uint32_t openGame();

  /** @brief Keeps the row of @p kill in the game numbered @p number, and adds
   * its points to the total of that game. */
  void addKill(std::uint32_t number, const game::EnemyKilled& kill);

  /** @brief Keeps the row of the total of the game numbered @p number, which
   * ended, and forgets that game. */
  void closeGame(std::uint32_t number);

  /** @returns The rows kept since the last call, oldest first, each ended by a
   * line feed; the next call starts afresh. */
  [[nodiscard]] std::string takeRows();

 private:
  std::uint32_t lastGame_{0};
  std::map<std::uint32_t, std::uint64_t> pointsOfOpenGames_;
  std::string rows_;
};

}  // namespace rtype::server
