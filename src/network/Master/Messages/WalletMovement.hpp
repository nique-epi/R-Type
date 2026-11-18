#pragma once

#include <chrono>
#include <cstdint>
#include "Currency.hpp"
#include "MovementReason.hpp"

namespace rtype::network {

/** @brief One line of the journal of a wallet: what moved one balance, kept
 * as it was written and never changed. */
struct WalletMovement {
  /** @brief Its number in the journal, which grows with each line. */
  std::int64_t id{};
  Currency currency{Currency::Kopecks};
  /** @brief What the balance gained, negative for what it lost. */
  std::int64_t amount{};
  MovementReason reason{MovementReason::Opening};
  std::chrono::sys_seconds madeAt;

  bool operator==(const WalletMovement&) const = default;
};

}  // namespace rtype::network
