#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include "ITicketLink.hpp"
#include "LatestValue.hpp"
#include "TicketAsks.hpp"
#include "TicketReading.hpp"

namespace rtype::client {

/**
 * @brief The asks for a game ticket on a thread of their own:
 * network::MasterAccountClient blocks for as long as HTTP takes,
 * MASTER_TRANSFER_TIMEOUT at most per call, and the frame loop must never wait
 * on it. The thread sleeps until a newer ask, serves the newest only, and
 * publishes what the master answered.
 *
 * Building it opens nothing; start() builds the client of the master, which
 * checks its url, and starts the thread, which owns it; stop() ends the
 * thread, a call in progress ended within the timeouts of the client. The
 * asks and the readings are safe from both threads.
 */
class MasterTicketLink final : public ITicketLink {
 public:
  MasterTicketLink() = default;
  ~MasterTicketLink() override = default;
  MasterTicketLink(const MasterTicketLink&) = delete;
  MasterTicketLink& operator=(const MasterTicketLink&) = delete;
  MasterTicketLink(MasterTicketLink&&) = delete;
  MasterTicketLink& operator=(MasterTicketLink&&) = delete;

  /** @brief Starts the thread on the master at @p url.
   * @throws network::InvalidMasterUrlException when the url is not
   * `http(s)://<host>[:<port>]`; the thread does not start then. */
  void start(const std::string& url);

  /** @brief Ends the thread, and with it its client. */
  void stop();

  std::uint64_t askTicket(const std::string& token) override;
  [[nodiscard]] std::optional<TicketReading> ticket() const override;

 private:
  TicketAsks asked_;
  concurrency::LatestValue<TicketAsks> asks_;
  concurrency::LatestValue<TicketReading> reading_;
  std::jthread thread_;
};

}  // namespace rtype::client
