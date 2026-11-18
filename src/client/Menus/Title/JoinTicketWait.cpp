#include "JoinTicketWait.hpp"
#include <optional>
#include <utility>
#include "IAccountSession.hpp"
#include "ITicketLink.hpp"
#include "JoinTicketWaitConstants.hpp"
#include "PendingJoin.hpp"
#include "TicketReading.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

JoinTicketWait::JoinTicketWait(const IAccountSession& account,
                               ITicketLink& tickets)
    : account_(&account), tickets_(&tickets) {}

std::optional<PendingJoin> JoinTicketWait::begin(const PendingJoin& join) {
  drop();
  if (!account_->signedIn()) {
    return join;
  }
  held_ = join;
  ask_ = tickets_->askTicket(account_->token());
  return std::nullopt;
}

void JoinTicketWait::drop() {
  held_.reset();
  waited_ = engine::Duration::zero();
}

bool JoinTicketWait::waiting() const { return held_.has_value(); }

std::optional<PendingJoin> JoinTicketWait::advance(engine::Duration elapsed) {
  if (!held_.has_value()) {
    return std::nullopt;
  }
  waited_ += elapsed;
  const std::optional<TicketReading> reading = tickets_->ticket();
  const bool answered = reading.has_value() && reading->ask == ask_;
  if (!answered && waited_ < TICKET_WAIT) {
    return std::nullopt;
  }
  PendingJoin ready = std::move(*held_);
  if (answered && reading->ticket.has_value()) {
    ready.request.ticket = *reading->ticket;
  }
  drop();
  return ready;
}

}  // namespace rtype::client
