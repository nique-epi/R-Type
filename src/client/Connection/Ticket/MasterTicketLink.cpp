#include "MasterTicketLink.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "GameTicket.hpp"
#include "GameTicketGrant.hpp"
#include "MasterAccountClient.hpp"
#include "MasterCalls.hpp"
#include "TicketAsks.hpp"
#include "TicketReading.hpp"

namespace rtype::client {

namespace {

/** @returns The ticket @p client gives the account of the token of @p asks;
 * none when the master refuses, does not answer or answers what cannot be
 * read. */
TicketReading ticketReadingOf(network::MasterAccountClient& client,
                              const TicketAsks& asks) {
  const std::optional<network::GameTicketGrant> grant = readingOrNothing(
      [&client, &asks] { return client.createOwnTicket(asks.token); });
  return TicketReading{.ask = asks.ask,
                       .ticket = grant.has_value()
                                     ? network::gameTicketOfText(grant->ticket)
                                     : std::nullopt};
}

}  // namespace

void MasterTicketLink::start(const std::string& url) {
  auto client = std::make_unique<network::MasterAccountClient>(url);
  thread_ = std::jthread(
      [this, client = std::move(client)](const std::stop_token& stop) {
        std::uint64_t served = 0;
        serveAsksUntilStopped(
            stop, asks_, [this, &client, &served](const TicketAsks& asks) {
              if (asks.ask > served) {
                reading_.publish(ticketReadingOf(*client, asks));
                served = asks.ask;
              }
            });
      });
}

void MasterTicketLink::stop() { thread_ = std::jthread{}; }

std::uint64_t MasterTicketLink::askTicket(const std::string& token) {
  ++asked_.ask;
  asked_.token = token;
  asks_.publish(asked_);
  return asked_.ask;
}

std::optional<TicketReading> MasterTicketLink::ticket() const {
  return reading_.value();
}

}  // namespace rtype::client
