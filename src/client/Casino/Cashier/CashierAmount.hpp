#pragma once

#include <cstddef>
#include <cstdint>
#include "CashierConstants.hpp"

namespace rtype::client {

/**
 * @brief The amount the cashier would change, and the step it moves by: one
 * of CASHIER_STEPS, chosen by its place. It opens on CASHIER_FIRST_AMOUNT and
 * the step CASHIER_FIRST_STEP, never falls below none and never passes
 * MAXIMUM_CHIPS.
 */
class CashierAmount {
 public:
  [[nodiscard]] std::uint32_t amount() const;
  /** @returns The place of the step in CASHIER_STEPS. */
  [[nodiscard]] std::size_t step() const;
  /** @brief Adds the step, MAXIMUM_CHIPS at most. */
  void raise();
  /** @brief Takes the step off, none at least. */
  void lower();
  /** @brief Moves by the step at @p place of CASHIER_STEPS from now on; a
   * place past the last changes nothing. */
  void chooseStep(std::size_t place);

 private:
  std::uint32_t amount_{CASHIER_FIRST_AMOUNT};
  std::size_t step_{CASHIER_FIRST_STEP};
};

}  // namespace rtype::client
