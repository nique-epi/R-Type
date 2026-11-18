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
  /** @brief The account the player played under, as a server older than the
   * game tickets reported it; none for a guest. The master never credits it:
   * a server cannot know an account, only the ticket a player hands it. */
  std::optional<std::string> accountId = std::nullopt;
  std::uint32_t score{};
  /** @brief The names of the look items the player won in the game, from
   * identifiers::SHIP_LOOK_ITEM_NAMES; empty when it won none, or when a
   * server older than the member does not say. */
  std::vector<std::string> wonLooks;
  /** @brief The game ticket the player joined with (see
   * isWellFormedGameTicket()); none for a guest, or when a server older than
   * the member does not say. */
  std::optional<std::string> ticket = std::nullopt;

  bool operator==(const FinishedGamePlayer&) const = default;
};

}  // namespace rtype::network
