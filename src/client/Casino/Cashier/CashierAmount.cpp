#include "CashierAmount.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include "ChipSelectorConstants.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client {

std::uint32_t CashierAmount::amount() const { return amount_; }

std::size_t CashierAmount::step() const { return step_; }

void CashierAmount::raise() {
  amount_ = static_cast<std::uint32_t>(std::min<std::uint64_t>(
      MAXIMUM_CHIPS, std::uint64_t{amount_} + CHIP_VALUES.at(step_)));
}

void CashierAmount::lower() {
  const std::uint32_t step = CHIP_VALUES.at(step_);
  amount_ = amount_ > step ? amount_ - step : 0;
}

void CashierAmount::chooseStep(std::size_t place) {
  if (place < CHIP_VALUES.size()) {
    step_ = place;
  }
}

}  // namespace rtype::client
