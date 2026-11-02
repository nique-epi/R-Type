#include "DatagramCodecFixtures.hpp"
#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <span>
#include <string>
#include <vector>
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

static_assert(SplittableAs<std::vector<std::byte>&>,
              "a datagram kept by the caller can be split");
static_assert(SplittableAs<const std::vector<std::byte>&>,
              "a constant datagram kept by the caller can be split");
static_assert(SplittableAs<std::span<const std::byte>>,
              "a view of a datagram kept by the caller can be split");
static_assert(!SplittableAs<std::vector<std::byte>>,
              "a temporary datagram cannot be split: its payloads would "
              "point into a destroyed buffer");
static_assert(!SplittableAs<const std::vector<std::byte>>,
              "a constant temporary datagram cannot be split either");
static_assert(!SplittableAs<std::array<std::byte, EMPTY_MESSAGE_DATAGRAM_SIZE>>,
              "a temporary array cannot be split either");

namespace {

/** @returns The bytes of a default header, less its last one. */
std::vector<std::byte> headerOneByteShort() {
  auto bytes = defaultHeaderBytes();
  bytes.pop_back();
  return bytes;
}

}  // namespace

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
