#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string_view>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "PlayerDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ThrottledLog.hpp"

namespace rtype::server {

/**
 * @brief Reads the datagrams of the players of a game: a datagram is readable
 * when its header carries this protocol and version with no flag set, its
 * messages add up to its end, its sender is in the roster and its sequence is
 * newer than the newest one accepted from that sender (ArrivalLedger), so a
 * duplicate or a late datagram is dropped before any of its messages is read.
 * Reading a datagram records its sequence.
 *
 * A datagram that is not readable is logged with its sender and why at the
 * debug level, at most once per REJECTION_LINE_INTERVAL of ticks counted, with
 * the number dropped since the previous line (see ThrottledLog), so a flood of
 * them cannot flood the log. The session token is not checked here: whoever
 * hands the datagrams over already dropped those that carry another token.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class DatagramReader {
 public:
  /**
   * @param roster The players whose datagrams are read; it must outlive the
   * reader.
   */
  explicit DatagramReader(PlayerRoster& roster);

  /**
   * @returns @p datagram split into its messages, with the player who sent it,
   * when it is readable; nothing otherwise, the datagram being dropped and
   * logged.
   */
  [[nodiscard]] std::optional<PlayerDatagram> read(
      const network::IncomingDatagram& datagram);

  /** @brief Counts one more tick, the clock of the lines it logs. */
  void countTick();

 private:
  void drop(const network::Endpoint& sender, std::string_view reason);

  /**
   * @returns The time the ticks counted stand for.
   */
  [[nodiscard]] std::chrono::nanoseconds timeRun() const;

  PlayerRoster& roster_;
  std::uint64_t ticksCounted_{0};
  logging::ThrottledLog rejectionLog_;
};

}  // namespace rtype::server
