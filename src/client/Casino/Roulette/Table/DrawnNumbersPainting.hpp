#pragma once

#include <cstdint>
#include <vector>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints the plaque of @p number under the wheel: a box of its color
 * framed in gold, the number twice as large, then its color and WINS beside
 * it. */
void paintResultPlaque(IInterfaceCanvas& canvas, std::uint8_t number);

/** @brief Paints the numbers drawn, @p numbers, the newest first: LAST, then
 * each in a box of its color, the newest framed in gold, the others in
 * bronze. Nothing when none was drawn. */
void paintLastNumbers(IInterfaceCanvas& canvas,
                      const std::vector<std::uint8_t>& numbers);

}  // namespace rtype::client
