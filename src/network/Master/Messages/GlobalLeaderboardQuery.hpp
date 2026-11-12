#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rtype::network {

/** @brief What a reader of `GET /api/leaderboard` asks for: the leaderboard
 * of one difficulty and one number of players on every server, every
 * difficulty or number of players when it names none, its first `limit` rows,
 * and the row of `player` when it names one. */
struct GlobalLeaderboardQuery {
  std::optional<std::string> difficulty = std::nullopt;
  std::optional<std::uint8_t> playerCount = std::nullopt;
  std::optional<std::string> player = std::nullopt;
  std::int64_t limit{};

  bool operator==(const GlobalLeaderboardQuery&) const = default;
};

}  // namespace rtype::network
