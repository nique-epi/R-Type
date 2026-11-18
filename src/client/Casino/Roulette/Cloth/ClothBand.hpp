#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief A band of places of the cloth, from the top: the numbers with the
 * zero at their head and the columns at their foot, the dozens under them,
 * then the even chances. */
enum class ClothBand : std::uint8_t {
  Numbers,
  Dozens,
  EvenChances,
};

}  // namespace rtype::client
