#pragma once

#include <cstddef>
#include "Purchase.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns The purchase of @p item by @p purse: the price taken and the item
 * in the locker, or the purse as it was and the first refusal that holds:
 * the locker holds the item, the shop does not sell it, it costs more than
 * the balance. */
[[nodiscard]] Purchase buyLookItem(const Purse& purse, std::size_t item);

}  // namespace rtype::client
