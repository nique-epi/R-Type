#pragma once

#include <vector>
#include "FlashLoan.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The pixels of the canvas that may show FLASH_COLOR, lent to the
 * effects that ask before they start, never more than FLASH_AREA_MAXIMUM at
 * once.
 *
 * An effect asks for the pixels its flash covers and for how long; refused,
 * it plays its version without flash. It is a guard: the count is made before
 * the effect starts, never measured on screen afterwards.
 */
class FlashBudget {
 public:
  /** @returns Whether the pixels are lent: true when they fit under
   * FLASH_AREA_MAXIMUM with every loan still running, which this one then
   * joins; false otherwise, and nothing is lent. */
  [[nodiscard]] bool reserve(FlashLoan loan);

  /** @brief Moves every loan on by the elapsed time; a loan whose time
   * passed gives its pixels back. */
  void advance(engine::Duration elapsed);

  /** @returns The pixels lent right now. */
  [[nodiscard]] int areaInUse() const;

 private:
  std::vector<FlashLoan> loans_;
};

}  // namespace rtype::client
