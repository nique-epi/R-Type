#pragma once

#include <cstddef>
#include <vector>
#include "ColoredPixel.hpp"

namespace rtype::client::testing {

/** @returns How many different places @p pixels covers. */
[[nodiscard]] std::size_t distinctPlacesOf(
    const std::vector<ColoredPixel>& pixels);

}  // namespace rtype::client::testing
