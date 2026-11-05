#pragma once

#include <cstddef>
#include <vector>
#include "IDatagramSocket.hpp"

namespace rtype::network::testing {

/**
 * @brief Keeps a copy of every datagram a socket hands to its handler.
 *
 * The handler it gives points at it: keep it alive as long as the socket.
 */
class ReceivedDatagrams {
 public:
  /** @returns A handler that appends each datagram it receives. */
  [[nodiscard]] DatagramHandler handler();

  /** @returns Every datagram received, in order. */
  [[nodiscard]] const std::vector<std::vector<std::byte>>& all() const;

 private:
  std::vector<std::vector<std::byte>> datagrams_;
};

}  // namespace rtype::network::testing
