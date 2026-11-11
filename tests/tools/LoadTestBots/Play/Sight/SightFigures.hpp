#pragma once

#include <cstdint>
#include <optional>

namespace rtype::tools {

/** @brief What a bot knows of its own ship and its run. */
struct SightFigures {
  /** @brief The tick of the newest state; nothing out of a game. */
  std::optional<std::uint32_t> tick;
  float shipX{};
  float shipY{};
  std::uint8_t lives{};
  bool invulnerable{false};
  /** @brief Whether its game runs and it still has a life. */
  bool inPlay{false};
  /** @brief The points of every game of the run: it only grows. */
  std::uint64_t scoreEarned{};
  /** @brief The lives lost in every game of the run: it only grows. */
  std::uint32_t livesLost{};
};

}  // namespace rtype::tools
