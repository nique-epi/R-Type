#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include <vector>
#include "MalformedDatagramBuilder.hpp"
#include "MalformedDatagramKind.hpp"
#include "MessageDispatcher.hpp"

namespace rtype::tools::testing {

/**
 * @brief A builder made from SOME_SEED, and a dispatcher that holds a handler
 * for every message type of the protocol, so a test sees whether the readers
 * refuse or skip what the builder writes.
 */
class MalformedDatagramBuilderFixture : public ::testing::Test {
 protected:
  MalformedDatagramBuilderFixture();

  /** @returns DRAWS_PER_KIND datagrams of @p kind, carrying SOME_HEADER. */
  [[nodiscard]] std::vector<std::vector<std::byte>> draw(
      MalformedDatagramKind kind);

  /**
   * @brief Splits @p datagram into messages and hands them to the dispatcher.
   * @throws MalformedDatagramException when it cannot be split.
   */
  void dispatch(std::span<const std::byte> datagram);

  /** @returns How many messages the reader of their type refused. */
  [[nodiscard]] std::size_t refusedCount() const;

  /** @returns How many messages had no reader. */
  [[nodiscard]] std::size_t skippedCount() const;

 private:
  MalformedDatagramBuilder builder_;
  network::MessageDispatcher dispatcher_;
};

}  // namespace rtype::tools::testing
