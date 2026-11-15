#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace rtype::client {

/** @brief What the band at the foot of the table shows but its bars: the
 * chips the player holds, its stake, the status line, and the place of the
 * chip chosen in CHIP_VALUES. */
struct RouletteBand {
  std::uint32_t chips{};
  std::uint32_t stake{};
  std::string status;
  std::size_t chosenChip{};
};

}  // namespace rtype::client
