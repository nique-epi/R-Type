#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

/** @brief How large the interface writes its text: as its fonts are drawn,
 * half again as large, or twice as large. */
enum class TextSize : std::uint8_t {
  Normal,
  Large,
  Largest,
};

/** @brief Every size, in the order the options list them. */
constexpr std::array TEXT_SIZES{TextSize::Normal, TextSize::Large,
                                TextSize::Largest};

constexpr std::size_t TEXT_SIZE_COUNT = TEXT_SIZES.size();

}  // namespace rtype::client
