#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"

namespace rtype::network::testing {

/**
 * @brief Whether DatagramCodec::splitMessages() takes a buffer of type Buffer:
 * it refuses one destroyed at the end of the call, whose payloads would point
 * into nothing.
 */
template <typename Buffer>
concept SplittableAs = requires(Buffer&& buffer) {
  DatagramCodec::splitMessages(std::forward<Buffer>(buffer));
};

/** @brief Bytes splitMessages() refuses, named after their fault. */
struct MalformedDatagram {
  const char* fault{};
  std::vector<std::byte> bytes;
};

/** @returns The bytes of a default header, less its last one. */
[[nodiscard]] std::vector<std::byte> headerOneByteShort();

/** @returns One datagram for each fault splitMessages() refuses. */
[[nodiscard]] std::vector<MalformedDatagram> malformedDatagrams();

/** @returns The fault of the datagram of @p info, as the name of its test. */
[[nodiscard]] std::string nameOfFault(
    const ::testing::TestParamInfo<MalformedDatagram>& info);

}  // namespace rtype::network::testing
