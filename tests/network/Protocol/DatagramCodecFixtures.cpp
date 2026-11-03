#include "DatagramCodecFixtures.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <ostream>
#include <string>
#include <vector>
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

void PrintTo(const MalformedDatagram& datagram, std::ostream* output) {
  *output << datagram.fault;
}

std::vector<std::byte> headerOneByteShort() {
  auto bytes = defaultHeaderBytes();
  bytes.pop_back();
  return bytes;
}

std::vector<MalformedDatagram> malformedDatagrams() {
  return {
      MalformedDatagram{.fault = "HeaderOneByteShort",
                        .bytes = headerOneByteShort()},
      MalformedDatagram{.fault = "HeaderAlone", .bytes = defaultHeaderBytes()},
      MalformedDatagram{.fault = "PrefixCutShort",
                        .bytes = datagramOf({HEARTBEAT_TYPE_BYTE, 0x00})},
      MalformedDatagram{
          .fault = "SizePastTheEnd",
          .bytes = datagramOf({READY_STATE_TYPE_BYTE, 0x00, 0x02, 0x01})},
      MalformedDatagram{
          .fault = "BytesAfterTheLastMessage",
          .bytes = datagramOf({READY_STATE_TYPE_BYTE, 0x00, 0x01, 0x01, 0x00})},
      MalformedDatagram{
          .fault = "SecondMessageCutShort",
          .bytes = datagramOf({READY_STATE_TYPE_BYTE, 0x00, 0x01, 0x01,
                               READY_STATE_TYPE_BYTE, 0x00, 0x01})}};
}

std::string nameOfFault(
    const ::testing::TestParamInfo<MalformedDatagram>& info) {
  return std::string{info.param.fault};
}

}  // namespace rtype::network::testing
