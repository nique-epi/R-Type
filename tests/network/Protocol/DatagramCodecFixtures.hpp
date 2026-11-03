#pragma once

#include <gtest/gtest.h>
#include <cstddef>
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

/** @returns The bytes of a default header without its last byte. */
std::vector<std::byte> headerOneByteShort();

class DatagramCodecMalformedDatagram
    : public ::testing::TestWithParam<MalformedDatagram> {};

}  // namespace rtype::network::testing
