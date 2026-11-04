#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "DatagramCodec.hpp"

namespace rtype::network::testing {

/** @brief Whether DatagramCodec::splitMessages() accepts a Buffer. */
template <typename Buffer>
concept SplittableAs = requires(Buffer&& buffer) {
  DatagramCodec::splitMessages(std::forward<Buffer>(buffer));
};

/** @brief Bytes splitMessages() refuses, named after their fault. */
struct MalformedDatagram {
  std::string_view fault;
  std::vector<std::byte> bytes;
};

/** @brief Prints a case by its fault, so the name CTest gives each test says
 * which case it runs, the same from one build to the next. */
void PrintTo(const MalformedDatagram& datagram, std::ostream* output);

/** @returns The bytes of a default header without its last byte. */
std::vector<std::byte> headerOneByteShort();

/** @returns One datagram for each fault splitMessages() refuses. */
[[nodiscard]] std::vector<MalformedDatagram> malformedDatagrams();

/** @returns The fault of the datagram of @p info, as the name of its test. */
[[nodiscard]] std::string nameOfFault(
    const ::testing::TestParamInfo<MalformedDatagram>& info);

}  // namespace rtype::network::testing
