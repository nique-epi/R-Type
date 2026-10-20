#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionRequest.hpp"
#include "DebugJournal.hpp"
#include "Endpoint.hpp"
#include "Heartbeat.hpp"
#include "MessageDispatcher.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"
#include "ReadyState.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network {
namespace {

using testing::byteSequence;
using testing::LARGEST_TYPE_BYTE;
using testing::oneShipState;
using testing::REJECTED_MESSAGE_COUNT;
using testing::REJECTED_MESSAGE_LINE_TEXT;
using testing::SOME_SENDER_PORT;
using testing::SOME_SESSION_TOKEN;
using testing::someSessionHeader;
using testing::TYPE_ABOVE_THE_LAST;
using testing::TYPE_BELOW_THE_FIRST;

MessageOrigin someOrigin() {
  return MessageOrigin{
      .sender = Endpoint{.address = std::string{LOOPBACK_IPV4_ADDRESS},
                         .port = SOME_SENDER_PORT},
      .header = someSessionHeader()};
}

MessageFrame frameOf(MessageType type, const std::vector<std::byte>& payload) {
  return MessageFrame{.type = static_cast<std::uint8_t>(type),
                      .payload = payload};
}

MessageFrame frameOf(std::uint8_t type, const std::vector<std::byte>& payload) {
  return MessageFrame{.type = type, .payload = payload};
}

/**
 * The ReadyState messages a dispatcher handed over, with their origins.
 */
struct ReadyStateRecord {
  std::vector<ReadyState> messages;
  std::vector<MessageOrigin> origins;
};

/**
 * Registers the ReadyState handler of @p dispatcher, which appends each
 * message and its origin to @p record; @p record must outlive the dispatches.
 */
void recordReadyStates(MessageDispatcher& dispatcher,
                       ReadyStateRecord& record) {
  dispatcher.registerHandler<ReadyState>(
      [&record](const MessageOrigin& origin, const ReadyState& message) {
        record.origins.push_back(origin);
        record.messages.push_back(message);
      });
}

/**
 * Given a handler for ReadyState
 * When a ReadyState message holding 01 is dispatched
 * Then the handler gets one ReadyState that is ready
 */
TEST(MessageDispatcher, HandsAMessageToTheHandlerOfItsType) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::ReadyState, ready)};

  dispatcher.dispatch(someOrigin(), frames);

  ASSERT_EQ(record.messages.size(), 1U);
  EXPECT_TRUE(record.messages.front().ready);
}

/**
 * Given a handler for ReadyState
 * When a ReadyState message is dispatched with an origin
 * Then the handler gets the sender and the session token of that origin
 */
TEST(MessageDispatcher, PassesTheOriginWithTheMessage) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::ReadyState, ready)};

  dispatcher.dispatch(someOrigin(), frames);

  ASSERT_EQ(record.origins.size(), 1U);
  EXPECT_EQ(record.origins.front().sender, someOrigin().sender);
  EXPECT_EQ(record.origins.front().header.sessionToken, SOME_SESSION_TOKEN);
}

/**
 * Given handlers for Heartbeat and ReadyState
 * When a Heartbeat then a ReadyState are dispatched
 * Then the handlers are called in that order
 */
TEST(MessageDispatcher, HandsSeveralMessagesInOrder) {
  MessageDispatcher dispatcher;
  std::vector<MessageType> handled;
  dispatcher.registerHandler<Heartbeat>(
      [&handled](const MessageOrigin&, const Heartbeat&) {
        handled.push_back(MessageType::Heartbeat);
      });
  dispatcher.registerHandler<ReadyState>(
      [&handled](const MessageOrigin&, const ReadyState&) {
        handled.push_back(MessageType::ReadyState);
      });
  const std::vector<std::byte> empty;
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::Heartbeat, empty),
                           frameOf(MessageType::ReadyState, ready)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(handled,
            (std::vector{MessageType::Heartbeat, MessageType::ReadyState}));
}

/**
 * Given a handler for ReadyState only
 * When messages of the types 0, 17 and 255, which no message has, then a
 * ReadyState are dispatched
 * Then the three are skipped and counted, and the ReadyState is still handled
 */
TEST(MessageDispatcher, SkipsUnknownTypesThenHandlesTheNextMessage) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);
  const std::vector<std::byte> empty;
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(TYPE_BELOW_THE_FIRST, empty),
                           frameOf(TYPE_ABOVE_THE_LAST, empty),
                           frameOf(LARGEST_TYPE_BYTE, empty),
                           frameOf(MessageType::ReadyState, ready)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(dispatcher.skippedMessageCount(), frames.size() - 1);
  EXPECT_EQ(record.messages.size(), 1U);
}

/**
 * Given a dispatcher without a handler for WorldState
 * When a WorldState message reaches it
 * Then it is skipped and counted, without an exception
 */
TEST(MessageDispatcher, SkipsATypeOfTheProtocolThatHasNoHandler) {
  MessageDispatcher dispatcher;
  const auto payload = WorldStateCodec::encode(oneShipState()).front();
  const std::vector frames{frameOf(MessageType::WorldState, payload)};

  EXPECT_NO_THROW(dispatcher.dispatch(someOrigin(), frames));

  EXPECT_EQ(dispatcher.skippedMessageCount(), 1U);
}

/**
 * Given a handler for ReadyState
 * When a ReadyState holding 02, then one holding 01, are dispatched
 * Then the first is dropped and counted, and only the second reaches the
 * handler
 */
TEST(MessageDispatcher, DropsAMalformedMessageThenHandlesTheNext) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);
  const auto invalid = byteSequence({0x02});
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::ReadyState, invalid),
                           frameOf(MessageType::ReadyState, ready)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(dispatcher.malformedMessageCount(), 1U);
  ASSERT_EQ(record.messages.size(), 1U);
  EXPECT_TRUE(record.messages.front().ready);
}

/**
 * Given a handler for ConnectionRequest
 * When a request cut to three bytes is dispatched
 * Then no exception leaves dispatch, the handler is not called, and the
 * message is counted as malformed
 */
TEST(MessageDispatcher, DropsATruncatedPayloadWithoutThrowing) {
  MessageDispatcher dispatcher;
  std::size_t handledCount = 0;
  dispatcher.registerHandler<ConnectionRequest>(
      [&handledCount](const MessageOrigin&, const ConnectionRequest&) {
        ++handledCount;
      });
  const auto truncated = byteSequence({0x00, 0x00, 0x00});
  const std::vector frames{frameOf(MessageType::ConnectionRequest, truncated)};

  EXPECT_NO_THROW(dispatcher.dispatch(someOrigin(), frames));

  EXPECT_EQ(handledCount, 0U);
  EXPECT_EQ(dispatcher.malformedMessageCount(), 1U);
}

/**
 * Given a handler for ConnectionAccepted that throws a network error
 * When a valid ConnectionAccepted is dispatched
 * Then the error leaves dispatch: it is a fault of the program
 */
TEST(MessageDispatcher, LetsAHandlerErrorThrough) {
  MessageDispatcher dispatcher;
  dispatcher.registerHandler<ConnectionAccepted>(
      [](const MessageOrigin&, const ConnectionAccepted&) {
        throw InvalidCoordinateException();
      });
  const auto playerOne = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::ConnectionAccepted, playerOne)};

  EXPECT_THROW(dispatcher.dispatch(someOrigin(), frames),
               InvalidCoordinateException);
}

/**
 * Given a handler for ConnectionAccepted that throws a network error
 * When the error leaves dispatch
 * Then the message is not counted as malformed
 */
TEST(MessageDispatcher, DoesNotCountAHandlerErrorAsMalformed) {
  MessageDispatcher dispatcher;
  dispatcher.registerHandler<ConnectionAccepted>(
      [](const MessageOrigin&, const ConnectionAccepted&) {
        throw InvalidCoordinateException();
      });
  const auto playerOne = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::ConnectionAccepted, playerOne)};

  EXPECT_THROW(dispatcher.dispatch(someOrigin(), frames),
               InvalidCoordinateException);

  EXPECT_EQ(dispatcher.malformedMessageCount(), 0U);
}

/**
 * Given a handler that threw on a first dispatch
 * When a second message is dispatched
 * Then the handler is called again
 */
TEST(MessageDispatcher, KeepsWorkingAfterAHandlerThrew) {
  MessageDispatcher dispatcher;
  std::size_t callCount = 0;
  dispatcher.registerHandler<ReadyState>(
      [&callCount](const MessageOrigin&, const ReadyState&) {
        ++callCount;
        if (callCount == 1) {
          throw InvalidCoordinateException();
        }
      });
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::ReadyState, ready)};
  EXPECT_THROW(dispatcher.dispatch(someOrigin(), frames),
               InvalidCoordinateException);

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(callCount, 2U);
}

/**
 * Given a handler for WorldState
 * When the WorldState payload of the protocol page is dispatched
 * Then the handler gets that state
 */
TEST(MessageDispatcher, HandsAWorldStateToItsHandler) {
  MessageDispatcher dispatcher;
  std::vector<WorldState> states;
  dispatcher.registerHandler<WorldState>(
      [&states](const MessageOrigin&, const WorldState& state) {
        states.push_back(state);
      });
  const auto payload = WorldStateCodec::encode(oneShipState()).front();
  const std::vector frames{frameOf(MessageType::WorldState, payload)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(states, std::vector{oneShipState()});
}

/**
 * Given a handler for ReadyState
 * When a second handler is registered for ReadyState
 * Then a DuplicateMessageHandlerException is raised
 */
TEST(MessageDispatcher, RefusesASecondHandlerForTheSameType) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);

  EXPECT_THROW(dispatcher.registerHandler<ReadyState>(
                   [](const MessageOrigin&, const ReadyState&) {}),
               DuplicateMessageHandlerException);
}

/**
 * Given a dispatcher
 * When an empty handler is registered
 * Then an EmptyMessageHandlerException is raised
 */
TEST(MessageDispatcher, RefusesAnEmptyHandler) {
  MessageDispatcher dispatcher;

  EXPECT_THROW(dispatcher.registerHandler<ReadyState>({}),
               EmptyMessageHandlerException);
}

/**
 * Given a handler for ReadyState
 * When no message is dispatched
 * Then the handler is not called and nothing is counted
 */
TEST(MessageDispatcher, DoesNothingForNoMessage) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);

  dispatcher.dispatch(someOrigin(), {});

  EXPECT_TRUE(record.messages.empty());
  EXPECT_EQ(dispatcher.skippedMessageCount(), 0U);
  EXPECT_EQ(dispatcher.malformedMessageCount(), 0U);
}

/** @brief Dispatcher tests that read the log the dispatcher writes. */
class MessageDispatcherLogTest : public ::testing::Test {
 protected:
  /** @returns The lines of the journal about a rejected message. */
  [[nodiscard]] std::vector<std::string> rejectionLines() const {
    return journal_.linesHolding(REJECTED_MESSAGE_LINE_TEXT);
  }

 private:
  DebugJournal journal_;
};

/**
 * Given a dispatcher without any handler
 * When a Heartbeat arrives from a sender
 * Then the journal holds a line about it that names the address and the port
 * of that sender
 */
TEST_F(MessageDispatcherLogTest, LogsASkippedMessageWithItsSender) {
  MessageDispatcher dispatcher;
  const std::vector<std::byte> payload = encodeMessage(Heartbeat{});
  const std::vector frames{frameOf(MessageType::Heartbeat, payload)};

  dispatcher.dispatch(someOrigin(), frames);

  const std::vector<std::string> lines = rejectionLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find("from " + std::string{LOOPBACK_IPV4_ADDRESS} +
                               " port " + std::to_string(SOME_SENDER_PORT)),
            std::string::npos);
}

/**
 * Given a handler for ReadyState
 * When REJECTED_MESSAGE_COUNT ReadyState messages holding 02 arrive at once
 * Then the journal holds one line about them, not one per message: whoever
 * floods the dispatcher cannot flood the log
 */
TEST_F(MessageDispatcherLogTest, WritesOneLineForMessagesRejectedTogether) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);
  const auto invalid = byteSequence({0x02});
  const std::vector<MessageFrame> frames(
      REJECTED_MESSAGE_COUNT, frameOf(MessageType::ReadyState, invalid));

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(rejectionLines().size(), 1U);
}

}  // namespace
}  // namespace rtype::network
