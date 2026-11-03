#include "DatagramCodecFixtures.hpp"
#include <cstddef>
#include <ostream>
#include <vector>
#include "ProtocolFixtures.hpp"

namespace rtype::network::testing {

void PrintTo(const MalformedDatagram& datagram, std::ostream* output) {
  *output << datagram.fault;
}

std::vector<std::byte> headerOneByteShort() {
  auto bytes = defaultHeaderBytes();
  bytes.pop_back();
  return bytes;
}

}  // namespace rtype::network::testing
