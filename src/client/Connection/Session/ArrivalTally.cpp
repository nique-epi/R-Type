#include "ArrivalTally.hpp"
#include <cstdint>
#include "SessionArrivals.hpp"

namespace rtype::client {

void ArrivalTally::countAccepted(std::uint16_t distance) {
  ++arrivals_.accepted;
  arrivals_.missed += distance - 1U;
}

SessionArrivals ArrivalTally::arrivals() const { return arrivals_; }

}  // namespace rtype::client
