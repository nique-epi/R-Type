#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief What a purchase of a look did. */
enum class LookPurchaseOutcome : std::uint8_t {
  /** @brief The price is taken and the look owned. */
  Bought,
  /** @brief The account owned it already: nothing taken. */
  AlreadyOwned,
  /** @brief The shop does not sell it. */
  NotForSale,
  /** @brief It costs more than the kopecks of the wallet. */
  NotEnoughKopecks,
};

}  // namespace rtype::master
