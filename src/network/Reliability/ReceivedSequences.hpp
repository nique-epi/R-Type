#pragma once

#include <cstdint>
#include <optional>
#include "PacketHeader.hpp"

namespace rtype::network {

/**
 * @brief The datagrams one side of a session accepted from its peer: the
 * sequence of the newest, and which of the ACKNOWLEDGEMENT_BIT_COUNT before it
 * were accepted too, as every datagram it sends back acknowledges them.
 *
 * A datagram is accepted when its sequence is newer than the newest accepted
 * (see isNewerSequence), so the sequences accepted only grow: a duplicate or a
 * late datagram is refused and never acknowledged, which makes its sender send
 * its guaranteed messages again.
 *
 * Not thread-safe: use it from one thread only.
 */
class ReceivedSequences {
 public:
  /**
   * @returns Sequences that accepted the datagram of @p sequence and nothing
   * else, as the client does with the acceptance of its session.
   */
  [[nodiscard]] static ReceivedSequences startingAt(std::uint16_t sequence);

  /**
   * @brief Accepts or refuses the datagram of sequence @p sequence.
   * @returns Whether it is newer than the newest accepted so far; the first
   * one always is. A refused datagram changes nothing.
   */
  [[nodiscard]] bool accept(std::uint16_t sequence);

  /** @returns The sequence of the newest datagram accepted; nothing before
   * the first. */
  [[nodiscard]] std::optional<std::uint16_t> newest() const;

  /**
   * @brief Writes the acknowledgement of the datagrams accepted into
   * @p header: ACKNOWLEDGES_FLAG, added to the flags it has, the newest in
   * acknowledgedSequence, and a set bit of acknowledgementBits for each of the
   * ACKNOWLEDGEMENT_BIT_COUNT before it that was accepted. Before the first
   * datagram it changes nothing, so the header acknowledges nothing.
   */
  void acknowledgeIn(PacketHeader& header) const;

 private:
  std::optional<std::uint16_t> newest_;
  std::uint32_t earlier_{0};
};

}  // namespace rtype::network
