#pragma once

#include "ClothBand.hpp"

namespace rtype::client {

/**
 * @brief A place of the cloth a bet lies on.
 *
 * In ClothBand::Numbers, `across` counts from the zero (0) to the columns
 * (NUMBERS_LAST_ACROSS): an odd value is a column of numbers, an even one the
 * line between two of them; `down` counts from the top: an even value is a
 * row of numbers, an odd one the line between two rows, STREET_EDGE_DOWN the
 * edge under the last row. On the zero, `down` keeps the row the cursor came
 * from. In ClothBand::Dozens and ClothBand::EvenChances, `across` is the
 * place in the band from the left, and `down` is 0.
 */
struct ClothPlace {
  ClothBand band{ClothBand::Numbers};
  int across{};
  int down{};

  bool operator==(const ClothPlace&) const = default;
};

}  // namespace rtype::client
