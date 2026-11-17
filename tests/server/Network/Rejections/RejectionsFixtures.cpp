#include "RejectionsFixtures.hpp"
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "Endpoint.hpp"
#include "EventField.hpp"
#include "RejectionCause.hpp"
#include "RejectionCounts.hpp"
#include "RejectionEvent.hpp"
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

std::size_t countOf(const rtype::server::RejectionCounts& counts,
                    rtype::server::RejectionCause cause) {
  return counts.at(static_cast<std::size_t>(cause));
}

rtype::server::RejectionCounts countsOf(
    std::initializer_list<rtype::server::RejectionCause> causes) {
  rtype::server::RejectionCounts counts{};
  for (const rtype::server::RejectionCause cause : causes) {
    rtype::server::addOneTo(counts, cause);
  }
  return counts;
}

std::vector<std::string> messagesOf(
    const std::vector<rtype::server::RejectionEvent>& events) {
  std::vector<std::string> messages;
  messages.reserve(events.size());
  for (const rtype::server::RejectionEvent& event : events) {
    messages.push_back(event.message);
  }
  return messages;
}

std::optional<rtype::logging::EventFieldValue> fieldOf(
    const rtype::server::RejectionEvent& event, std::string_view name) {
  for (const rtype::logging::EventField& field : event.fields) {
    if (field.name == name) {
      return field.value;
    }
  }
  return std::nullopt;
}
