#pragma once

#include <vector>
#include "MessageFrame.hpp"
#include "PacketHeader.hpp"
#include "ReceptionKind.hpp"

namespace rtype::client {

/**
 * @brief A datagram of the server, checked: what to do with it, its header,
 * and the messages it holds when ReceptionKind::Read. The messages point into
 * the datagram, which stays valid until the next drain of the inbound side.
 */
struct Reception {
  ReceptionKind kind{};
  network::PacketHeader header;
  std::vector<network::MessageFrame> messages;
};

}  // namespace rtype::client
