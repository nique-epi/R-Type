#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rtype::network {

/** @brief What one player left of a finished game, as a server reports it. */
struct FinishedGamePlayer {
  /** @brief A nickname of the protocol. */
  std::string nickname;
  /** @brief The account the player played under; none for a guest. */
  std::optional<std::string> accountId = std::nullopt;
  std::uint32_t score{};

  bool operator==(const FinishedGamePlayer&) const = default;
};

}  // namespace rtype::network
