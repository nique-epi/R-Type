#pragma once

#include <cstdint>
#include <optional>
#include "ArrivalTally.hpp"
#include "PacketHeader.hpp"
#include "ReceivedSequences.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the client acknowledges to the server within a session: the
 * datagrams of the server it accepted (see network::ReceivedSequences), and
 * how long the acknowledgement of guaranteed messages has waited for a
 * datagram to carry it.
 *
 * SessionInbound accepts each datagram of the session through it, and
 * SessionOutbox writes the acknowledgement into each datagram it sends, which
 * ends the wait. Accepting a datagram that holds guaranteed messages starts
 * the wait; another one does not start it again, so the oldest
 * acknowledgement sets when a Heartbeat is due. Outside a session nothing is
 * accepted, so nothing is acknowledged and nothing waits. Each datagram
 * accepted within a session is counted in an ArrivalTally, with the sequences
 * it skipped.
 *
 * Not thread-safe: use it from one thread only.
 */
class SessionAcknowledgements {
 public:
  /** @param arrivals Counts the datagrams accepted; it must outlive the
   * acknowledgements. */
  explicit SessionAcknowledgements(ArrivalTally& arrivals);

  /** @brief Opens the session at its acceptance: the datagram of sequence
   * @p acceptanceSequence counts as accepted, and nothing waits. */
  void open(std::uint16_t acceptanceSequence);

  /** @brief Closes the session: nothing is accepted any more, and nothing
   * waits. */
  void close();

  /**
   * @brief Accepts or refuses the datagram of @p header.
   * @returns Whether its sequence is newer than the newest accepted. The
   * wait starts when it is and the datagram holds guaranteed messages, and
   * the tally counts it, with the sequences skipped since the newest one.
   */
  [[nodiscard]] bool accept(const network::PacketHeader& header);

  /** @brief Writes the acknowledgement of the datagrams accepted into
   * @p header, and ends the wait, since @p header carries it. */
  void acknowledgeIn(network::PacketHeader& header);

  /** @brief Counts @p elapsed of waiting, while an acknowledgement waits. */
  void advance(engine::Duration elapsed);

  /** @returns Whether an acknowledgement waited ACKNOWLEDGEMENT_DELAY, so a
   * Heartbeat must carry it. */
  [[nodiscard]] bool isAcknowledgementDue() const;

 private:
  ArrivalTally* arrivals_;
  network::ReceivedSequences sequences_;
  std::optional<engine::Duration> wait_;
};

}  // namespace rtype::client
