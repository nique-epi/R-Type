#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief Why the cashier refuses an exchange. */
enum class WalletExchangeRefusal : std::uint8_t {
  /** @brief An amount of none, or past the largest balance. */
  InvalidAmount,
  /** @brief More than the balance it leaves. */
  NotEnough,
  /** @brief The balance it comes into would pass its largest value. */
  PastTheMaximum,
};

}  // namespace rtype::network
