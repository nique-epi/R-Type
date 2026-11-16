#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace rtype::network {

/** @returns The price of @p item, a position in
 * identifiers::SHIP_LOOK_ITEM_NAMES, in kopecks; NOT_FOR_SALE for an item the
 * shop does not sell. */
[[nodiscard]] std::uint32_t lookItemPrice(std::size_t item);

/** @returns The position in identifiers::SHIP_LOOK_ITEM_NAMES of the item
 * named @p name, compared byte for byte; nothing for a name no item has. */
[[nodiscard]] std::optional<std::size_t> lookItemOfName(std::string_view name);

/** @returns @p items, names of looks, in the order of
 * identifiers::SHIP_LOOK_ITEM_NAMES; a name of no look goes last. */
[[nodiscard]] std::vector<std::string> inCatalogueOrder(
    std::vector<std::string> items);

}  // namespace rtype::network
