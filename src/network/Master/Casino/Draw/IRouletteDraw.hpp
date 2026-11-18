#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Where a roulette table takes the number of a round.
 */
class IRouletteDraw {
 public:
  virtual ~IRouletteDraw() = default;

  /** @returns A number of the wheel, 0 to HIGHEST_ROULETTE_NUMBER. */
  [[nodiscard]] virtual std::uint8_t drawNumber() = 0;
};

}  // namespace rtype::network
