#pragma once

#include <cstdint>
#include <optional>
#include "ChipRefusal.hpp"

namespace rtype::master {

/** @brief What IAccountWallet::applyChips() did. */
struct ChipMovementResult {
  /** @brief Why nothing moved; nothing when the movement applied. */
  std::optional<ChipRefusal> refusal;
  /** @brief The chips of the wallet after the movement, or as they were when
   * it was refused; 0 for an unknown account. */
  std::uint32_t chips{};
};

}  // namespace rtype::master
