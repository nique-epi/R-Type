#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include "GameResult.hpp"

namespace rtype::server::testing {

/** @returns A vanilla game of one pilot, @p pilot, at the normal difficulty,
 * ended at @p endedAt. */
[[nodiscard]] GameResult vanillaGameOf(std::string_view pilot,
                                       std::chrono::sys_seconds endedAt);

/** @brief Saves @p result in the file of results @p file, as a game process
 * does.
 * @returns The id the file drew for it. */
[[nodiscard]] std::string saveResultIn(const std::filesystem::path& file,
                                       const GameResult& result);

/** @brief What the file of results says the master answered on a game. */
struct MasterAnswers {
  /** @brief Whether every row of the game holds a sent_at. */
  bool sent{false};
  /** @brief Whether every row of the game holds a refused_at. */
  bool refused{false};
};

/** @returns What @p file says the master answered on the game @p gameId;
 * neither for a game it does not hold. */
[[nodiscard]] MasterAnswers answersIn(const std::filesystem::path& file,
                                      std::string_view gameId);

}  // namespace rtype::server::testing
