#include "FakeTicketLink.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "GameTicket.hpp"
#include "TicketReading.hpp"

std::uint64_t FakeTicketLink::askTicket(const std::string& token) {
  askedTokens_.push_back(token);
  return askedTokens_.size();
}

std::optional<rtype::client::TicketReading> FakeTicketLink::ticket() const {
  return reading_;
}

void FakeTicketLink::answerLastAsk(
    std::optional<rtype::network::GameTicket> ticket) {
  reading_ = rtype::client::TicketReading{.ask = askedTokens_.size(),
                                          .ticket = std::move(ticket)};
}

const std::vector<std::string>& FakeTicketLink::askedTokens() const {
  return askedTokens_;
}
