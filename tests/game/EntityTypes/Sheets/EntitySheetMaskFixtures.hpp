#pragma once

#include <cstddef>
#include <string>

namespace rtype::game::testing {

/** @returns A table `[gameplay.mask]` of one row of @p width drawn pixels. */
[[nodiscard]] std::string maskOfOneRow(std::size_t width);

}  // namespace rtype::game::testing
