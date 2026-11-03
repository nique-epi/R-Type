#include "DatagramCodecFixtures.hpp"
#include <cstddef>
#include <vector>
#include "ProtocolFixtures.hpp"

namespace rtype::network::testing {

std::vector<std::byte> headerOneByteShort() {
  auto bytes = defaultHeaderBytes();
  bytes.pop_back();
  return bytes;
}

}  // namespace rtype::network::testing
