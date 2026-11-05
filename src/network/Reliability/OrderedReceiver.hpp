#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "EncodedMessage.hpp"
#include "OrderedMessage.hpp"
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief The receiving end of the ordered channel of a session: it hands the
 * messages carried by OrderedMessage envelopes over in the order of their
 * numbers, once each, and keeps those that arrive early until the ones before
 * them arrive or their sender gives them up.
 *
 * It holds at most ORDERED_WINDOW messages, whatever it receives. A message
 * never delays anything but the channel: the caller handles every other
 * message of a datagram as it arrives.
 *
 * Not thread-safe: use it from one thread only.
 */
class OrderedReceiver {
 public:
  /**
   * @brief Takes in @p message and returns the messages that can now be
   * handled, oldest first: those held before message.firstKept, which the
   * sender gave up waiting for, then the unbroken run from the next number
   * expected. A number already handled, or ORDERED_WINDOW or more ahead of the
   * next one expected, is ignored.
   */
  [[nodiscard]] std::vector<EncodedMessage> receive(
      const OrderedMessage& message);

  /**
   * @brief Forgets every message held and expects FIRST_ORDERED_NUMBER again:
   * a new session numbers from the start.
   */
  void reset();

  /** @returns How many messages wait for an earlier one. */
  [[nodiscard]] std::size_t heldCount() const;

 private:
  /** @brief Moves the number expected to @p firstKept when that is later,
   * handing the messages held before it to @p ready. */
  void skipTo(std::uint16_t firstKept, std::vector<EncodedMessage>& ready);

  /** @brief Keeps @p message until its turn, unless it is already handled, too
   * far ahead or held. */
  void hold(const OrderedMessage& message);

  /** @brief Hands the unbroken run of held messages from the number expected
   * to @p ready. */
  void deliverRun(std::vector<EncodedMessage>& ready);

  [[nodiscard]] std::optional<EncodedMessage>& slotOf(std::uint16_t number);

  std::array<std::optional<EncodedMessage>, ORDERED_WINDOW> slots_;
  std::uint16_t expected_{FIRST_ORDERED_NUMBER};
};

}  // namespace rtype::network
