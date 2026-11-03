#pragma once

#include <gtest/gtest.h>
#include "DatagramCodecFixtures.hpp"

namespace rtype::network::testing {

/** @brief The tests run once for each datagram splitMessages() refuses. */
class DatagramCodecFixture
    : public ::testing::TestWithParam<MalformedDatagram> {};

}  // namespace rtype::network::testing
