#include "ArrivalLedger.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ArrivalLedgerConstants.hpp"
#include "SequenceOrder.hpp"

namespace rtype::server {

namespace {

constexpr std::size_t FIRST_DATAGRAM_DISTANCE = 1;

}  // namespace

bool ArrivalLedger::accept(std::uint16_t sequence) {
  if (newest_.has_value() && !network::isNewerSequence(sequence, *newest_)) {
    return false;
  }
  expected_ += newest_.has_value()
                   ? static_cast<std::uint16_t>(sequence - *newest_)
                   : FIRST_DATAGRAM_DISTANCE;
  ++received_;
  newest_ = sequence;
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
