#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>
#include "BoundedQueue.hpp"
#include "BoundedQueueTestConstants.hpp"
#include "EngineException.hpp"

using rtype::engine::BoundedQueue;
using rtype::engine::InvalidQueueCapacityException;
using rtype::engine::QueueableMessage;

namespace {

std::vector<int> numbered(int first, std::size_t count) {
  std::vector<int> messages;
  messages.reserve(count);
  for (std::size_t offset = 0; offset < count; ++offset) {
    messages.push_back(first + static_cast<int>(offset));
  }
  return messages;
}

void pushAll(BoundedQueue<int>& queue, const std::vector<int>& messages) {
  for (const int message : messages) {
    queue.push(message);
  }
}

/**
 * @brief A message whose move constructor and move assignment may throw.
 */
struct MessageWithThrowingMove {
  int value{0};

  MessageWithThrowingMove() = default;
  MessageWithThrowingMove(const MessageWithThrowingMove&) = default;
  MessageWithThrowingMove& operator=(const MessageWithThrowingMove&) = default;
  // NOLINTNEXTLINE(performance-noexcept-move-constructor)
  MessageWithThrowingMove(MessageWithThrowingMove&& other) noexcept(false)
      : value(std::move(other).value) {}
  // NOLINTNEXTLINE(performance-noexcept-move-constructor)
  MessageWithThrowingMove& operator=(MessageWithThrowingMove&& other) noexcept(
      false) {
    value = std::move(other).value;
    return *this;
  }
  ~MessageWithThrowingMove() = default;
};

std::vector<int> drained(BoundedQueue<int>& queue) {
  std::vector<int> messages;
  queue.drainInto(messages);
  return messages;
}

}  // namespace

/**
 * Given a queue with room for three messages
 * When three messages are pushed, then drained
 * Then they come out in the order they were pushed
 */
TEST(BoundedQueue, DrainsMessagesInTheOrderTheyWerePushed) {
  BoundedQueue<int> queue{SMALL_CAPACITY};
  const std::vector<int> messages = numbered(1, SMALL_CAPACITY);

  pushAll(queue, messages);

  EXPECT_EQ(drained(queue), messages);
}

/**
 * Given a queue nothing was pushed to
 * When it is drained
 * Then nothing comes out
 */
TEST(BoundedQueue, DrainingAnEmptyQueueGivesNothing) {
  BoundedQueue<int> queue{SMALL_CAPACITY};

  EXPECT_TRUE(drained(queue).empty());
}

/**
 * Given a queue holding messages that was drained once
 * When it is drained again
 * Then nothing comes out
 */
TEST(BoundedQueue, DrainingEmptiesTheQueue) {
  BoundedQueue<int> queue{SMALL_CAPACITY};
  pushAll(queue, numbered(1, SMALL_CAPACITY));
  drained(queue);

  EXPECT_TRUE(drained(queue).empty());
}

/**
 * Given a destination already holding a message
 * When a queue holding one message is drained into it
 * Then the destination holds its earlier message, then the drained one
 */
TEST(BoundedQueue, DrainingKeepsWhatTheDestinationAlreadyHeld) {
  BoundedQueue<int> queue{SMALL_CAPACITY};
  queue.push(1);
  std::vector<int> destination{EARLIER_MESSAGE};

  queue.drainInto(destination);

  EXPECT_EQ(destination, (std::vector<int>{EARLIER_MESSAGE, 1}));
}

/**
 * Given a queue with room for three messages
 * When five messages are pushed, then drained
 * Then only the three newest come out, oldest first
 */
TEST(BoundedQueue, FullQueueDiscardsItsOldestMessage) {
  BoundedQueue<int> queue{SMALL_CAPACITY};

  pushAll(queue, numbered(1, SMALL_CAPACITY + EXTRA_MESSAGE_COUNT));

  EXPECT_EQ(drained(queue), numbered(1 + static_cast<int>(EXTRA_MESSAGE_COUNT),
                                     SMALL_CAPACITY));
}

/**
 * Given a queue with room for three messages
 * When five messages are pushed
 * Then two messages are counted as discarded
 */
TEST(BoundedQueue, CountsEveryDiscardedMessage) {
  BoundedQueue<int> queue{SMALL_CAPACITY};

  pushAll(queue, numbered(1, SMALL_CAPACITY + EXTRA_MESSAGE_COUNT));

  EXPECT_EQ(queue.discardedCount(), EXTRA_MESSAGE_COUNT);
}

/**
 * Given a queue with room for three messages
 * When exactly three messages are pushed
 * Then no message is counted as discarded
 */
TEST(BoundedQueue, CountsNothingWhileThereIsRoom) {
  BoundedQueue<int> queue{SMALL_CAPACITY};

  pushAll(queue, numbered(1, SMALL_CAPACITY));

  EXPECT_EQ(queue.discardedCount(), 0U);
}

/**
 * Given a queue that discarded one message, then was drained
 * When it is filled again, then drained
 * Then the new messages come out in the order they were pushed
 */
TEST(BoundedQueue, KeepsTheOrderAfterDiscardingAndDraining) {
  BoundedQueue<int> queue{SMALL_CAPACITY};
  pushAll(queue, numbered(1, SMALL_CAPACITY + 1));
  drained(queue);
  const std::vector<int> secondRound =
      numbered(SECOND_ROUND_FIRST_MESSAGE, SMALL_CAPACITY);

  pushAll(queue, secondRound);

  EXPECT_EQ(drained(queue), secondRound);
}

/**
 * Given a queue that discarded one message
 * When it is drained, then filled again without overflowing
 * Then the discard is still counted, and only once
 */
TEST(BoundedQueue, DrainingKeepsTheDiscardCount) {
  BoundedQueue<int> queue{SMALL_CAPACITY};
  pushAll(queue, numbered(1, SMALL_CAPACITY + 1));
  drained(queue);

  pushAll(queue, numbered(SECOND_ROUND_FIRST_MESSAGE, SMALL_CAPACITY));

  EXPECT_EQ(queue.discardedCount(), 1U);
}

/**
 * Given a queue with room for a single message
 * When three messages are pushed, then drained
 * Then only the newest comes out
 */
TEST(BoundedQueue, KeepsOnlyTheNewestMessageWithRoomForOne) {
  BoundedQueue<int> queue{SINGLE_MESSAGE_CAPACITY};
  const std::vector<int> messages = numbered(1, SMALL_CAPACITY);

  pushAll(queue, messages);

  EXPECT_EQ(drained(queue), (std::vector<int>{messages.back()}));
}

/**
 * Given a queue of messages that can be moved but not copied
 * When one is pushed, then drained
 * Then the drained message carries the value pushed
 */
TEST(BoundedQueue, CarriesMessagesThatCannotBeCopied) {
  BoundedQueue<std::unique_ptr<int>> queue{SMALL_CAPACITY};
  queue.push(std::make_unique<int>(CARRIED_VALUE));
  std::vector<std::unique_ptr<int>> messages;

  queue.drainInto(messages);

  ASSERT_EQ(messages.size(), 1U);
  EXPECT_EQ(*messages.front(), CARRIED_VALUE);
}

/**
 * Given a message type whose move may throw
 * When it is checked against what a queue requires of its messages
 * Then it is refused, so a queue of that type does not compile
 */
TEST(BoundedQueue, RefusesMessagesWhoseMoveMayThrow) {
  EXPECT_FALSE(QueueableMessage<MessageWithThrowingMove>);
  EXPECT_TRUE(QueueableMessage<int>);
}

/**
 * Given no queue
 * When a queue with room for no message is created
 * Then InvalidQueueCapacityException is thrown
 */
TEST(BoundedQueue, RefusesAZeroCapacity) {
  EXPECT_THROW(BoundedQueue<int>{0}, InvalidQueueCapacityException);
}

/**
 * Given a queue with room for every message, and a reader thread draining it
 * in a loop
 * When the writer thread pushes 100 000 numbered messages
 * Then the reader received every message once, in the order pushed
 */
TEST(BoundedQueue, LosesNothingWhileAnotherThreadDrains) {
  BoundedQueue<int> queue{CONCURRENT_MESSAGE_COUNT};
  const std::vector<int> messages = numbered(0, CONCURRENT_MESSAGE_COUNT);
  std::vector<int> received;
  std::jthread reader{[&queue, &received](const std::stop_token& stop) {
    while (!stop.stop_requested()) {
      queue.drainInto(received);
    }
  }};

  pushAll(queue, messages);
  reader.request_stop();
  reader.join();
  queue.drainInto(received);

  EXPECT_EQ(received, messages);
}

/**
 * Given a queue with room for eight messages, and a reader thread draining it
 * in a loop
 * When the writer thread pushes 100 000 numbered messages
 * Then every message was either received or counted as discarded, and the
 * received ones are in the order pushed, each once
 */
TEST(BoundedQueue, AccountsForEveryMessageWhenItOverflowsUnderTwoThreads) {
  BoundedQueue<int> queue{OVERFLOWING_CAPACITY};
  std::vector<int> received;
  std::jthread reader{[&queue, &received](const std::stop_token& stop) {
    while (!stop.stop_requested()) {
      queue.drainInto(received);
    }
  }};

  pushAll(queue, numbered(0, CONCURRENT_MESSAGE_COUNT));
  reader.request_stop();
  reader.join();
  queue.drainInto(received);

  EXPECT_EQ(received.size() + queue.discardedCount(), CONCURRENT_MESSAGE_COUNT);
  EXPECT_TRUE(std::ranges::is_sorted(received));
  EXPECT_EQ(std::ranges::adjacent_find(received), received.end());
}
