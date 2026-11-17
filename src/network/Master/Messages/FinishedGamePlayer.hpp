#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace rtype::network {

/** @brief What one player left of a finished game, as a server reports it. */
struct FinishedGamePlayer {
  /** @brief A nickname of the protocol. */
  std::string nickname;
  /** @brief The account the player played under; none for a guest. */
  std::optional<std::string> accountId = std::nullopt;
  std::uint32_t score{};
  /** @brief The names of the look items the player won in the game, from
   * identifiers::SHIP_LOOK_ITEM_NAMES; empty when it won none, or when a
   * server older than the member does not say. */
  std::vector<std::string> wonLooks;

  bool operator==(const FinishedGamePlayer&) const = default;
};

}  // namespace rtype::network
