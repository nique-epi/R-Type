#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include "Purchase.hpp"
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns The price of @p item, a position in SHIP_LOOK_ITEM_NAMES, in
 * kopecks; NOT_FOR_SALE for an item the shop does not sell. */
[[nodiscard]] std::uint32_t lookItemPrice(std::size_t item);

/** @returns Why @p purse cannot buy @p item, the first that holds: the locker
 * holds it, the shop does not sell it, it costs more than the balance;
 * nothing when it can. */
[[nodiscard]] std::optional<PurchaseRefusal> purchaseRefusalOf(
    const Purse& purse, std::size_t item);

/** @returns The purchase of @p item by @p purse: the price taken and the item
 * in the locker, or the refusal and the purse as it was. */
[[nodiscard]] Purchase buyLookItem(const Purse& purse, std::size_t item);

}  // namespace rtype::client
