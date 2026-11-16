#pragma once

#include <cstddef>
#include <optional>
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns Why @p purse cannot buy @p item, the first refusal that holds:
 * the locker holds the item, the shop does not sell it, it costs more than
 * the balance; nothing when it can, the master then making the purchase. */
[[nodiscard]] std::optional<PurchaseRefusal> lookRefusalOf(const Purse& purse,
                                                           std::size_t item);

}  // namespace rtype::client
