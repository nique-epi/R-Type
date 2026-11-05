#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "MessageDispatcher.hpp"
#include "MessageType.hpp"
#include "OrderedInbox.hpp"

namespace rtype::client::testing {

/**
 * @brief An OrderedInbox on a dispatcher whose handlers of GameEvent,
 * EntityDestroyed and Heartbeat note the type of each message they handle, in
 * the order they handle them.
 */
class OrderedInboxFixture : public ::testing::Test {
 protected:
  OrderedInboxFixture();

  /** @brief Dispatches the message of the ordered channel numbered
   * @p number, keeping from @p firstKept: a GameEvent for the first number, an
   * EntityDestroyed for any other. */
  void deliverOrdered(std::uint16_t number, std::uint16_t firstKept);

  /** @brief Dispatches a Heartbeat, which is not on the ordered channel. */
  void deliverHeartbeat();

  /** @brief Tells the inbox the session closed. */
  void closeSession();

  /** @returns The types of the messages handled so far, in order. */
  [[nodiscard]] const std::vector<network::MessageType>& handled() const;

 private:
  network::MessageDispatcher dispatcher_;
  OrderedInbox inbox_{dispatcher_};
  std::vector<network::MessageType> handled_;
};

}  // namespace rtype::client::testing
