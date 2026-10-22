#include "FlashBudget.hpp"
#include <vector>
#include "EffectConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

bool FlashBudget::reserve(Loan loan) {
  if (areaInUse() + loan.area > FLASH_AREA_MAXIMUM) {
    return false;
  }
  loans_.push_back(loan);
  return true;
}

void FlashBudget::advance(engine::Duration elapsed) {
  for (Loan& loan : loans_) {
    loan.remaining -= elapsed;
  }
  std::erase_if(loans_, [](const Loan& loan) {
    return loan.remaining <= engine::Duration::zero();
  });
}

int FlashBudget::areaInUse() const {
  int area = 0;
  for (const Loan& loan : loans_) {
    area += loan.area;
  }
  return area;
}

}  // namespace rtype::client
