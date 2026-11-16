#pragma once

#include <cstdint>
#include <string>
#include <thread>
#include "CasinoAsks.hpp"
#include "CasinoLinkState.hpp"
#include "CasinoReadings.hpp"
#include "CasinoSeat.hpp"
#include "ICasinoLink.hpp"
#include "RoomRound.hpp"
#include "RouletteBets.hpp"

namespace rtype::client {

/**
 * @brief The calls to the casino of the master on a thread of their own:
 * network::MasterCasinoClient blocks for as long as HTTP takes,
 * MASTER_TRANSFER_TIMEOUT at most per call, and the frame loop must never wait
 * on it.
 *
 * The frame loop publishes its asks; the thread sleeps until a newer one, and
 * serves the newest of each kind only: the seat first, so bets follow a seat
 * taken, then the bets, the state of the room, the bets of a round drawn and
 * the rooms. A master that does not answer, refuses or writes what cannot be
 * read gives a reading that says so, never no reading.
 *
 * Building it opens nothing; start() builds the client of the master, which
 * checks its url, and starts the thread, which owns it; stop() ends the
 * thread, a call in progress ended within the timeouts of the client. The
 * asks and the readings are safe from both threads.
 */
class MasterCasinoLink final : public ICasinoLink {
 public:
  MasterCasinoLink() = default;
  ~MasterCasinoLink() override = default;
  MasterCasinoLink(const MasterCasinoLink&) = delete;
  MasterCasinoLink& operator=(const MasterCasinoLink&) = delete;
  MasterCasinoLink(MasterCasinoLink&&) = delete;
  MasterCasinoLink& operator=(MasterCasinoLink&&) = delete;

  /** @brief Starts the thread on the master at @p url.
   * @throws network::InvalidMasterUrlException when the url is not
   * `http(s)://<host>[:<port>]`; the thread does not start then. */
  void start(const std::string& url);

  /** @brief Ends the thread, and with it its client. */
  void stop();

  void askRooms() override;
  std::uint64_t askRoom(std::uint8_t room) override;
  std::uint64_t askSeat(const std::string& token, CasinoSeat seat) override;
  std::uint64_t askBets(const std::string& token, network::RoomRound round,
                        const network::RouletteBets& bets) override;
  std::uint64_t askOwnBets(const std::string& token,
                           network::RoomRound round) override;
  [[nodiscard]] CasinoReadings readings() const override;

 private:
  CasinoAsks asked_;
  CasinoLinkState state_;
  std::jthread thread_;
};

}  // namespace rtype::client
