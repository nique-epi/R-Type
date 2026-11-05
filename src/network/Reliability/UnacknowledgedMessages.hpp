#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "UnacknowledgedMessage.hpp"

namespace rtype::network {

/**
 * @brief The guaranteed messages one side sent its peer and the peer has not
 * acknowledged yet, kept to be sent again, and numbered: they form the ordered
 * channel of the session, each one sent in an OrderedMessage that carries its
 * number and firstKept, the number of the oldest message still kept, so a
 * receiver hands them over in order and skips a message given up (see
 * OrderedReceiver).
 *
 * A message is due when it was never sent, or was last sent
 * GUARANTEED_RESEND_DELAY ago or more. It leaves once an acknowledgement names
 * one of the datagrams it went in, or once it is GUARANTEED_MESSAGE_LIFETIME
 * old, acknowledged or not: what it shows no longer holds by then. A message
 * is also given up once ORDERED_WINDOW numbers were given after it. Giving a
 * message up moves firstKept past it.
 *
 * Not thread-safe: use it from one thread only.
 */
class UnacknowledgedMessages {
 public:
  /**
   * @brief Keeps @p message, due at once.
   * @throws DatagramTooLargeException when @p message does not fit a datagram
   * alone once enveloped, so takeDue() always has room for the first message
   * due; nothing is kept then.
   */
  void add(EncodedMessage message);

  /**
   * @brief Drops every message one of whose datagrams @p header acknowledges
   * (see acknowledges()). @p header comes from a datagram of the peer that
   * passed every check, so a forged one acknowledges nothing.
   */
  void acknowledge(const PacketHeader& header);

  /**
   * @brief Makes every message @p elapsed older, and gives up those that reach
   * GUARANTEED_MESSAGE_LIFETIME.
   */
  void advance(std::chrono::nanoseconds elapsed);

  /** @returns Whether a message is due. */
  [[nodiscard]] bool hasDue() const;

  /**
   * @brief Takes the due messages that fit one datagram, oldest first, and
   * records that they go in the datagram of sequence @p sequence.
   * @returns Their OrderedMessage envelopes, at least one while hasDue() is
   * true, none otherwise.
   */
  [[nodiscard]] std::vector<EncodedMessage> takeDue(std::uint16_t sequence);

  /** @returns How many messages are kept. */
  [[nodiscard]] std::size_t count() const;

 private:
  /** @brief Gives up every kept message numbered before @p first, and moves
   * firstKept_ to @p first when that is later than it is. */
  void giveUpBefore(std::uint16_t first);

  /** @returns The envelope that sends @p kept now, under the current
   * firstKept_. */
  [[nodiscard]] EncodedMessage envelopeOf(
      const UnacknowledgedMessage& kept) const;

  std::vector<UnacknowledgedMessage> messages_;
  std::uint16_t nextNumber_{FIRST_ORDERED_NUMBER};
  std::uint16_t firstKept_{FIRST_ORDERED_NUMBER};
};

}  // namespace rtype::network
