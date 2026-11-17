#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace rtype::server {

/** @brief What one player leaves of a game. */
struct PlayerResult {
  std::string nickname;
  std::uint32_t score{};
  /** @brief The lives the player lost in the game. */
  std::uint32_t deaths{};
  /** @brief The names of the look items the player won in the game, from
   * identifiers::SHIP_LOOK_ITEM_NAMES, each once, in the order it won them. */
  std::vector<std::string> wonLooks;
  /** @brief The game ticket the player joined with, as the master writes it
   * (network::gameTicketText()); none for a guest. */
  std::optional<std::string> ticket = std::nullopt;

  bool operator==(const PlayerResult&) const = default;
};

/**
 * @brief What one finished game leaves: a row per player, the players who left
 * before its end included.
 */
struct GameResult {
  std::string difficulty;
  /** @brief The players connected when the game started. */
  std::uint8_t playerCount{};
  /** @brief When the game ended, to the second. */
  std::chrono::sys_seconds endedAt;
  std::vector<PlayerResult> players;
  /** @brief Whether the game was vanilla (see GameSettings::vanilla). */
  bool vanilla{false};
  /** @brief Whether its players cleared the level (GameOutcome::Victory). */
  bool victory{false};
};

}  // namespace rtype::server
