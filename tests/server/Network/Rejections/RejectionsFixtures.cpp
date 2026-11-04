#include "RejectionsFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include "Endpoint.hpp"
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
#include "RejectionLedger.hpp"
#include "RejectionsTestConstants.hpp"

rtype::network::Endpoint rejectedSender(std::size_t index) {
  return rtype::network::Endpoint{
      .address = std::string{REJECTED_ADDRESS},
      .port = static_cast<std::uint16_t>(FIRST_REJECTED_PORT + index)};
}

void recordStranger(rtype::server::RejectionLedger& ledger, std::size_t index) {
  ledger.record(rtype::server::RejectionCause::Stranger, rejectedSender(index));
}

void recordStrangers(rtype::server::RejectionLedger& ledger,
                     std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    recordStranger(ledger, index);
  }
}

rtype::server::RejectionCounts countsOf(
    std::initializer_list<rtype::server::RejectionCause> causes) {
  rtype::server::RejectionCounts counts{};
  for (const rtype::server::RejectionCause cause : causes) {
    rtype::server::addOneTo(counts, cause);
  }
  return counts;
}
