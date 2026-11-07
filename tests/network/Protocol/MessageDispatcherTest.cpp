#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include <vector>
#include "ByteSequence.hpp"
#include "ConnectionAccepted.hpp"
#include "ConnectionRequest.hpp"
#include "Heartbeat.hpp"
#include "LogLineTestConstants.hpp"
#include "MessageDispatcher.hpp"
#include "MessageDispatcherFixture.hpp"
#include "MessageDispatcherFixtures.hpp"
#include "MessageFrame.hpp"
#include "MessageOrigin.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "NetworkTestConstants.hpp"
#include "ProtocolTestConstants.hpp"
#include "ReadyState.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network {

using testing::acceptanceOfPlayerOne;
using testing::byteSequence;
using testing::FAILED_HANDLER_LINE_TEXT;
using testing::frameOf;
using testing::LARGEST_TYPE_BYTE;
using testing::MessageDispatcherFixture;
using testing::oneShipState;
using testing::ReadyStateRecord;
using testing::recordReadyStates;
using testing::registerThrowingHandler;
using testing::REJECTED_MESSAGE_COUNT;
using testing::SOME_SESSION_TOKEN;
using testing::someOrigin;
using testing::someSenderText;
using testing::TYPE_ABOVE_THE_LAST;
using testing::TYPE_BELOW_THE_FIRST;

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
  const std::vector<std::byte> heartbeat = encodeMessage(Heartbeat{});
  const auto ready = byteSequence({0x01});
  const std::vector frames{frameOf(MessageType::Heartbeat, heartbeat),
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
 * Then the message is counted as dropped because its handler threw
 */
TEST(MessageDispatcher, CountsAMessageWhoseHandlerThrew) {
  MessageDispatcher dispatcher;
  registerThrowingHandler<ConnectionAccepted>(dispatcher);
  const auto playerOneWithOneType = acceptanceOfPlayerOne();
  const std::vector frames{
      frameOf(MessageType::ConnectionAccepted, playerOneWithOneType)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(dispatcher.failedMessageCount(), 1U);
}

/**
 * Given a handler for ConnectionAccepted that throws a network error
 * When a valid ConnectionAccepted is dispatched
 * Then the message is not counted as malformed
 */
TEST(MessageDispatcher, DoesNotCountAHandlerErrorAsMalformed) {
  MessageDispatcher dispatcher;
  registerThrowingHandler<ConnectionAccepted>(dispatcher);
  const auto playerOneWithOneType = acceptanceOfPlayerOne();
  const std::vector frames{
      frameOf(MessageType::ConnectionAccepted, playerOneWithOneType)};

  dispatcher.dispatch(someOrigin(), frames);

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
  dispatcher.dispatch(someOrigin(), frames);

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(callCount, 2U);
}

/**
 * Given a ReadyState handler that throws a network error, and a Heartbeat
 * handler
 * When a ReadyState then a Heartbeat are dispatched together
 * Then the Heartbeat is still handled: only the message whose handler threw
 * is dropped
 */
TEST(MessageDispatcher, HandlesTheMessageAfterOneWhoseHandlerThrew) {
  MessageDispatcher dispatcher;
  registerThrowingHandler<ReadyState>(dispatcher);
  std::size_t heartbeatCount = 0;
  dispatcher.registerHandler<Heartbeat>(
      [&heartbeatCount](const MessageOrigin&, const Heartbeat&) {
        ++heartbeatCount;
      });
  const auto ready = byteSequence({0x01});
  const std::vector<std::byte> heartbeat = encodeMessage(Heartbeat{});
  const std::vector frames{frameOf(MessageType::ReadyState, ready),
                           frameOf(MessageType::Heartbeat, heartbeat)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(heartbeatCount, 1U);
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

/**
 * Given a dispatcher without any handler
 * When a Heartbeat arrives from a sender
 * Then the journal holds a line about it that names the address and the port
 * of that sender
 */
TEST_F(MessageDispatcherFixture, LogsASkippedMessageWithItsSender) {
  MessageDispatcher dispatcher;
  const std::vector<std::byte> payload = encodeMessage(Heartbeat{});
  const std::vector frames{frameOf(MessageType::Heartbeat, payload)};

  dispatcher.dispatch(someOrigin(), frames);

  const std::vector<std::string> lines = rejectionLines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(someSenderText()), std::string::npos);
}

/**
 * Given a dispatcher without any handler
 * When a Heartbeat arrives
 * Then the journal holds one line holding SKIPPED_MESSAGE_LINE_TEXT, the words
 * the tests that expect no such line look for
 */
TEST_F(MessageDispatcherFixture, WritesTheWordsOfASkippedMessage) {
  MessageDispatcher dispatcher;
  const std::vector<std::byte> payload = encodeMessage(Heartbeat{});
  const std::vector frames{frameOf(MessageType::Heartbeat, payload)};

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(linesHolding(SKIPPED_MESSAGE_LINE_TEXT).size(), 1U);
}

/**
 * Given a handler for ReadyState
 * When REJECTED_MESSAGE_COUNT ReadyState messages holding 02 arrive at once
 * Then the journal holds one line about them, not one per message: whoever
 * floods the dispatcher cannot flood the log
 */
TEST_F(MessageDispatcherFixture, WritesOneLineForMessagesRejectedTogether) {
  MessageDispatcher dispatcher;
  ReadyStateRecord record;
  recordReadyStates(dispatcher, record);
  const auto invalid = byteSequence({0x02});
  const std::vector<MessageFrame> frames(
      REJECTED_MESSAGE_COUNT, frameOf(MessageType::ReadyState, invalid));

  dispatcher.dispatch(someOrigin(), frames);

  EXPECT_EQ(rejectionLines().size(), 1U);
}

/**
 * Given a handler for ConnectionAccepted that throws a network error
 * When a valid ConnectionAccepted is dispatched
 * Then the journal holds one line about it at the error level
 */
TEST_F(MessageDispatcherFixture, LogsAThrowingHandlerAtTheErrorLevel) {
  MessageDispatcher dispatcher;
  registerThrowingHandler<ConnectionAccepted>(dispatcher);
  const auto playerOneWithOneType = acceptanceOfPlayerOne();
  const std::vector frames{
      frameOf(MessageType::ConnectionAccepted, playerOneWithOneType)};

  dispatcher.dispatch(someOrigin(), frames);

  const std::vector<std::string> lines = linesHolding(FAILED_HANDLER_LINE_TEXT);
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(ERROR_LINE_MARK), std::string::npos);
}

/**
 * Given a handler for ConnectionAccepted that throws a network error
 * When a valid ConnectionAccepted is dispatched
 * Then its line names the address and the port of the sender
 */
TEST_F(MessageDispatcherFixture, LogsAThrowingHandlerWithItsSender) {
  MessageDispatcher dispatcher;
  registerThrowingHandler<ConnectionAccepted>(dispatcher);
  const auto playerOneWithOneType = acceptanceOfPlayerOne();
  const std::vector frames{
      frameOf(MessageType::ConnectionAccepted, playerOneWithOneType)};

  dispatcher.dispatch(someOrigin(), frames);

  const std::vector<std::string> lines = linesHolding(FAILED_HANDLER_LINE_TEXT);
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(someSenderText()), std::string::npos);
}

}  // namespace rtype::network
