#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"
#include "PacketHeader.hpp"
#include "UnacknowledgedMessage.hpp"

namespace rtype::network {

/**
 * @brief The guaranteed messages one side sent its peer and the peer has not
 * acknowledged yet, kept to be sent again.
 *
 * A message is due when it was never sent, or was last sent
 * GUARANTEED_RESEND_DELAY ago or more. It leaves once an acknowledgement names
 * one of the datagrams it went in, or once it is GUARANTEED_MESSAGE_LIFETIME
 * old, acknowledged or not: what it shows no longer holds by then. A receiver
 * may get a message twice, when the acknowledgement of the first copy is lost,
 * so a guaranteed message is one its receiver can read twice; the protocol
 * page lists them.
 *
 * Not thread-safe: use it from one thread only.
 */
class UnacknowledgedMessages {
 public:
  /**
   * @brief Keeps @p message, due at once.
   * @throws DatagramTooLargeException when @p message does not fit a datagram
   * alone, so takeDue() always has room for the first message due; nothing is
   * kept then.
   */
  void add(EncodedMessage message);

  /**
   * @brief Drops every message one of whose datagrams @p header acknowledges
   * (see acknowledges()). @p header comes from a datagram of the peer that
   * passed every check, so a forged one acknowledges nothing.
   */
  void acknowledge(const PacketHeader& header);

  /**
   * @brief Makes every message @p elapsed older, and drops those that reach
   * GUARANTEED_MESSAGE_LIFETIME.
   */
  void advance(std::chrono::nanoseconds elapsed);

  /** @returns Whether a message is due. */
  [[nodiscard]] bool hasDue() const;

  /**
   * @brief Takes the due messages that fit one datagram, oldest first, and
   * records that they go in the datagram of sequence @p sequence.
   * @returns At least one message while hasDue() is true, none otherwise.
   */
  [[nodiscard]] std::vector<EncodedMessage> takeDue(std::uint16_t sequence);

  /** @returns How many messages are kept. */
  [[nodiscard]] std::size_t count() const;

 private:
  std::vector<UnacknowledgedMessage> messages_;
};

}  // namespace rtype::network
