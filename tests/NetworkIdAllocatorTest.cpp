#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <set>
#include "EngineException.hpp"
#include "NetworkIdAllocator.hpp"
#include "NetworkIdConstants.hpp"

using rtype::engine::NetworkIdAllocator;
using rtype::engine::NetworkIdExhaustedException;

constexpr std::uint32_t FIRST_NETWORK_ID = 1;
constexpr int MANY_ALLOCATIONS = 1000;

/**
 * Given a new allocator
 * When an identifier is allocated
 * Then it is 1
 */
TEST(NetworkIdAllocator, FirstIdentifierIsOne) {
  NetworkIdAllocator allocator;

  EXPECT_EQ(allocator.allocate(), FIRST_NETWORK_ID);
}

/**
 * Given a new allocator
 * When many identifiers are allocated
 * Then they are all different and none is the reserved identifier
 */
TEST(NetworkIdAllocator, IdentifiersAreUniqueAndNeverReserved) {
  NetworkIdAllocator allocator;
  std::set<std::uint32_t> seen;

  for (int count = 0; count < MANY_ALLOCATIONS; ++count) {
    seen.insert(allocator.allocate());
  }

  EXPECT_EQ(seen.size(), static_cast<std::size_t>(MANY_ALLOCATIONS));
  EXPECT_EQ(seen.count(rtype::engine::NO_NETWORK_ID), 0U);
}

/**
 * Given an allocator that already gave the last identifier but one
 * When the last identifier is allocated
 * Then it is the largest value of 32 bits
 */
TEST(NetworkIdAllocator, LastIdentifierIsTheLargestValue) {
  constexpr std::uint32_t largest = std::numeric_limits<std::uint32_t>::max();
  NetworkIdAllocator allocator(largest - 1);

  EXPECT_EQ(allocator.allocate(), largest);
}

/**
 * Given an allocator that gave every identifier
 * When another one is requested
 * Then it throws instead of wrapping around to the reserved identifier
 */
TEST(NetworkIdAllocator, ExhaustionThrows) {
  NetworkIdAllocator allocator(std::numeric_limits<std::uint32_t>::max());

  EXPECT_THROW(allocator.allocate(), NetworkIdExhaustedException);
}
