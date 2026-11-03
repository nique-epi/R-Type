#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include "EncodedMessage.hpp"

namespace rtype::network {

/**
 * @brief A guaranteed message its receiver has not acknowledged yet: the
 * message, how long ago it was given, how long ago it was last sent, and the
 * sequence of every datagram it went in, any of which an acknowledgement may
 * name. It was never sent while sentIn is empty.
 */
struct UnacknowledgedMessage {
  EncodedMessage message;
  std::chrono::nanoseconds age{};
  std::chrono::nanoseconds sinceSent{};
  std::vector<std::uint16_t> sentIn;
};

}  // namespace rtype::network
