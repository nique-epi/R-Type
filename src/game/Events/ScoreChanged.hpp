#pragma once

#include <cstdint>

namespace rtype::game {

/**
 * @brief Event published when the score of a player grows: its number, the
 * one of the protocol, 1 to 4, its score before and its score after, which
 * the ScoreKeeper of the game wrote.
 */
struct ScoreChanged {
  std::uint8_t playerNumber{};
  std::uint32_t before{};
  std::uint32_t after{};

  bool operator==(const ScoreChanged&) const = default;
};

}  // namespace rtype::game
