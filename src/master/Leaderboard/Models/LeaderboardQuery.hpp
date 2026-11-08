#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "PageRequest.hpp"

namespace rtype::master {

/** @brief What a request reads of the leaderboard: one difficulty and one
 * number of players, never mixed with another, then a page of it. */
struct LeaderboardQuery {
  std::string difficulty;
  std::uint8_t playerCount{};
  /** @brief The server whose rows alone are read; every server when none. */
  std::optional<std::int64_t> serverId = std::nullopt;
  /** @brief The nickname whose row is read too, wherever it ranks; none when
   * the request names none. */
  std::optional<std::string> player = std::nullopt;
  PageRequest page;
};

}  // namespace rtype::master
