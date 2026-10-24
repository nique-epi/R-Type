#include "FlashBudget.hpp"
#include <vector>
#include "EffectConstants.hpp"
#include "FlashLoan.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

bool FlashBudget::reserve(FlashLoan loan) {
  if (areaInUse() + loan.area > FLASH_AREA_MAXIMUM) {
    return false;
  }
  loans_.push_back(loan);
  return true;
}

void FlashBudget::advance(engine::Duration elapsed) {
  for (FlashLoan& loan : loans_) {
    loan.remaining -= elapsed;
  }
  std::erase_if(loans_, [](const FlashLoan& loan) {
    return loan.remaining <= engine::Duration::zero();
  });
}

int FlashBudget::areaInUse() const {
  int area = 0;
  for (const FlashLoan& loan : loans_) {
    area += loan.area;
  }
  return area;
}

}  // namespace rtype::client
