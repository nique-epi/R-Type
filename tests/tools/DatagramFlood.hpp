#pragma once

#include <cstddef>
#include <cstdint>
#include "FloodConstants.hpp"
#include "FloodRecipient.hpp"
#include "FloodReport.hpp"
#include "LoopbackPeer.hpp"
#include "MalformedDatagramBuilder.hpp"
#include "PacketHeader.hpp"

namespace rtype::tools {

/**
 * @brief Sends a recipient FLOOD_DATAGRAMS_PER_SECOND malformed datagrams per
 * second for FLOOD_DURATION, the kinds of MALFORMED_DATAGRAM_KINDS in turn.
 *
 * Each datagram carries the session token of the recipient and the next
 * sequence, so a server that admitted the tool, or a client that took it for
 * its server, reads it as far as the fault. The datagrams that can be split
 * into messages keep the session open: the recipient never times the tool
 * out during a flood.
 */
class DatagramFlood {
 public:
  /**
   * @param seed The seed of the datagrams: the same seed sends the same
   * datagrams in the same order.
   */
  DatagramFlood(FloodRecipient recipient, std::uint32_t seed);

  /**
   * @brief Sends the flood through @p peer, and meanwhile reads and counts
   * what the recipient sends.
   * @returns What it sent and received. Call it once.
   */
  [[nodiscard]] FloodReport run(LoopbackPeer& peer);

 private:
  void sendNext(LoopbackPeer& peer);
  [[nodiscard]] network::PacketHeader nextHeader();

  FloodRecipient recipient_;
  MalformedDatagramBuilder builder_;
  FloodReport report_;
  std::size_t sentCount_{0};
  std::uint16_t sequence_{SEQUENCE_BEFORE_THE_FLOOD};
};

}  // namespace rtype::tools
