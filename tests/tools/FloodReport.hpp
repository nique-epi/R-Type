#pragma once

#include <array>
#include <cstddef>
#include "MalformedDatagramConstants.hpp"

namespace rtype::tools {

/**
 * @brief What a flood did: how many datagrams of each kind it sent, at the
 * place of the kind in MALFORMED_DATAGRAM_KINDS, how many the system refused
 * to send, and how many the recipient sent meanwhile.
 */
struct FloodReport {
  std::array<std::size_t, MALFORMED_DATAGRAM_KINDS.size()> sentByKind{};
  std::size_t refusedBySystemCount{};
  std::size_t receivedCount{};
};

}  // namespace rtype::tools
