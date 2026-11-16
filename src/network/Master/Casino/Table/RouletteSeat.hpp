#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief One player seated at the table: the chips of its bets this round,
 * and the chips the table hands back once the number is drawn (0 until then,
 * and for a lost round).
 */
struct RouletteSeat {
  std::uint8_t playerNumber{};
  std::uint32_t stake{};
  std::uint32_t payout{};

  bool operator==(const RouletteSeat&) const = default;
};

}  // namespace rtype::network
