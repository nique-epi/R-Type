#include <gtest/gtest.h>
#include <vector>
#include "MessageType.hpp"
#include "OrderedInboxFixture.hpp"
#include "OrderedInboxTestConstants.hpp"

namespace rtype::client::testing {

using network::MessageType;

/**
 * Given the death of a player, number 2, arrives before the hit that caused
 * it, number 1
 * When the hit arrives
 * Then the hit is handled first, then the death
 */
TEST_F(OrderedInboxFixture, HandlesAHitBeforeTheDeathItCaused) {
  deliverOrdered(SECOND_ORDERED, FIRST_ORDERED);
  ASSERT_TRUE(handled().empty());

  deliverOrdered(FIRST_ORDERED, FIRST_ORDERED);

  EXPECT_EQ(handled(),
            (std::vector<MessageType>{MessageType::GameEvent,
                                      MessageType::EntityDestroyed}));
}

/**
 * Given the message number 1 of the ordered channel is missing
 * When a Heartbeat, which is not on the channel, arrives
 * Then it is handled at once
 */
TEST_F(OrderedInboxFixture, DoesNotDelayAMessageOutsideTheChannel) {
  deliverOrdered(SECOND_ORDERED, FIRST_ORDERED);

  deliverHeartbeat();

  EXPECT_EQ(handled(), (std::vector<MessageType>{MessageType::Heartbeat}));
}

/**
 * Given a message handled once
 * When the same message arrives again
 * Then it is not handled a second time
 */
TEST_F(OrderedInboxFixture, HandlesACopyOfAMessageOnce) {
  deliverOrdered(FIRST_ORDERED, FIRST_ORDERED);

  deliverOrdered(FIRST_ORDERED, FIRST_ORDERED);

  EXPECT_EQ(handled(), (std::vector<MessageType>{MessageType::GameEvent}));
}

/**
 * Given a message handled in a session that then closed
 * When the new session sends its number 1
 * Then it is handled: a new session numbers from the start
 */
TEST_F(OrderedInboxFixture, ExpectsTheFirstNumberAgainInANewSession) {
  deliverOrdered(FIRST_ORDERED, FIRST_ORDERED);
  closeSession();

  deliverOrdered(FIRST_ORDERED, FIRST_ORDERED);

  EXPECT_EQ(handled(), (std::vector<MessageType>{MessageType::GameEvent,
                                                 MessageType::GameEvent}));
}

}  // namespace rtype::client::testing
