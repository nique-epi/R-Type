#pragma once

#include <cstdint>
#include <string>

namespace rtype::network {

/**
 * @brief The rules the creator of a game sets for that game alone: the
 * difficulty, a table of the waves file of the server; the lives each player
 * starts with; whether a missile of a player costs another player a life. The
 * supervisor checks the difficulty and the lives, and refuses the request when
 * they do not fit (see RefusalReason).
 */
struct GameRules {
  std::string difficulty;
  std::uint8_t lives{0};
  bool friendlyFire{false};

  bool operator==(const GameRules&) const = default;
};

/**
 * @returns The rules of a game nobody chose rules for: DEFAULT_DIFFICULTY,
 * DEFAULT_LIVES and no friendly fire.
 */
[[nodiscard]] GameRules defaultGameRules();

}  // namespace rtype::network
