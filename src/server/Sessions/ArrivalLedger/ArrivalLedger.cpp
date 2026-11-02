#include "ArrivalLedger.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ArrivalLedgerConstants.hpp"

namespace rtype::server {

bool ArrivalLedger::accept(std::uint16_t sequence) {
  const std::optional<std::uint16_t> previous = sequences_.newest();
  if (!sequences_.accept(sequence)) {
    return false;
  }
  expected_ += previous.has_value()
                   ? static_cast<std::uint16_t>(sequence - *previous)
                   : FIRST_DATAGRAM_DISTANCE;
  ++received_;
  closeWindowWhenFull();
  return true;
}

std::optional<double> ArrivalLedger::lossRate() const { return lossRate_; }

void ArrivalLedger::closeWindowWhenFull() {
  if (expected_ < LOSS_WINDOW_SIZE) {
    return;
  }
  lossRate_ = static_cast<double>(expected_ - received_) /
              static_cast<double>(expected_);
  expected_ = 0;
  received_ = 0;
}

}  // namespace rtype::server
