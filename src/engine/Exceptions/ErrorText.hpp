#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "DataFilePlace.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::engine {

/** @returns Where @p place is, as compilers write it: `file:line:column`. */
[[nodiscard]] std::string placeText(const DataFilePlace& place);

/** @returns @p names separated by commas, or `none` when there is no name. */
[[nodiscard]] std::string listText(std::span<const std::string_view> names);

/** @returns @p names as the other listText() writes them. */
[[nodiscard]] std::string listText(const std::vector<std::string>& names);

/** @returns The text of the whole numbers of @p range: `1 to 100`. */
[[nodiscard]] std::string rangeText(WholeNumberRange range);

}  // namespace rtype::engine
