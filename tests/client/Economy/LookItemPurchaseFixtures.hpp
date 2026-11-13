#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Purse.hpp"

namespace rtype::client::testing {

/** @returns The position of the item named @p name in SHIP_LOOK_ITEM_NAMES;
 * SHIP_LOOK_ITEM_COUNT when no item has that name. */
[[nodiscard]] std::size_t itemNamed(std::string_view name);

/** @returns A purse of @p kopecks whose locker holds no item. */
[[nodiscard]] Purse purseWithAnEmptyLocker(std::uint32_t kopecks);

}  // namespace rtype::client::testing
