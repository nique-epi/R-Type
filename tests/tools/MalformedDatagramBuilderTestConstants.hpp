#pragma once

#include <cstddef>
#include <cstdint>
#include "PacketHeader.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::tools::testing {

/**
 * @brief The seed of the builder of every test.
 */
constexpr std::uint32_t SOME_SEED = 1;

/**
 * @brief How many datagrams of a kind each test draws: enough to reach every
 * sample message and every bounded field many times over.
 */
constexpr std::size_t DRAWS_PER_KIND = 1000;

/**
 * @brief The header every drawn datagram is given: a session token and a
 * sequence, every other field at its default.
 */
constexpr network::PacketHeader SOME_HEADER{
    .sessionToken = network::testing::SOME_SESSION_TOKEN,
    .sequence = network::testing::SOME_SEQUENCE};

}  // namespace rtype::tools::testing
