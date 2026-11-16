#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include "ChatChannelConstants.hpp"
#include "ChatChannelFixture.hpp"
#include "ChatChannelTestConstants.hpp"
#include "ChatEntry.hpp"
#include "ChatLine.hpp"
#include "LobbyState.hpp"
#include "ServerSessionTestConstants.hpp"

namespace rtype::client {

using testing::ACCEPTED_PLAYER_NUMBER;
using testing::CHAT_INTERVAL;
using testing::ChatChannelFixture;
using testing::COPIES_BEFORE_GIVING_UP;
using ::testing::Each;
using ::testing::Field;
using testing::FILLER_LETTER;
using testing::FIRST_LINE;
using testing::FIRST_TEXT;
using testing::FRAMES_UNTIL_GIVING_UP;
using ::testing::IsEmpty;
using testing::LINES_KEPT;
using testing::LINES_PAST_THE_LIMIT;
using testing::LINES_TAKEN_AT_ONCE;
using testing::NOVA;
using testing::NOVA_LINE;
using testing::NOVA_NUMBER;
using testing::OLDEST_KEPT;
using testing::SECOND_LINE;
using testing::SECOND_TEXT;
using testing::SHORT_FRAME;
using testing::SMALLEST_TIME;
using testing::TOO_LONG_TEXT_SIZE;

/**
 * Given no session
 * When a line is sent
 * Then it is refused and nothing leaves
 */
TEST_F(ChatChannelFixture, WithoutASessionALineIsRefused) {
  EXPECT_FALSE(chat().send(FIRST_TEXT));

  EXPECT_THAT(sentLines(), IsEmpty());
}

/**
 * Given an open session
 * When a line is sent
 * Then it leaves at once as line 1
 */
TEST_F(ChatChannelFixture, ALineLeavesAtOnceAsLineOne) {
  connectAndGetAccepted();

  EXPECT_TRUE(chat().send(FIRST_TEXT));

  ASSERT_EQ(sentLines().size(), 1U);
  EXPECT_EQ(sentLines().front(),
            (network::ChatLine{.lineNumber = FIRST_LINE,
                               .text = std::string{FIRST_TEXT}}));
}

/**
 * Given a line sent and not relayed back
 * When just under, then exactly, half a second goes by
 * Then no copy leaves before, one copy of the same number after
 */
TEST_F(ChatChannelFixture, AnUnansweredLineIsSentAgainEveryHalfSecond) {
  connectAndGetAccepted();
  ASSERT_TRUE(chat().send(FIRST_TEXT));

  update(CHAT_INTERVAL - SMALLEST_TIME);
  EXPECT_EQ(sentLines().size(), 1U);
  update(SMALLEST_TIME);

  EXPECT_EQ(sentLines().size(), 2U);
  EXPECT_THAT(sentLines(),
              Each(Field(&network::ChatLine::lineNumber, FIRST_LINE)));
}

/**
 * Given a line sent
 * When the server relays it back with the number of this player
 * Then it joins the chat and no copy leaves any more
 */
TEST_F(ChatChannelFixture, TheLineRelayedBackEndsItsCopies) {
  connectAndGetAccepted();
  ASSERT_TRUE(chat().send(FIRST_TEXT));

  relayLine(FIRST_LINE, FIRST_TEXT, ACCEPTED_PLAYER_NUMBER);
  update(CHAT_INTERVAL);

  EXPECT_EQ(sentLines().size(), 1U);
  ASSERT_EQ(chat().entries().size(), 1U);
  EXPECT_EQ(chat().entries().front().text, FIRST_TEXT);
}

/**
 * Given a first line sent, then a second
 * When the first is relayed back
 * Then the second leaves only then, as line 2
 */
TEST_F(ChatChannelFixture, ASecondLineWaitsForTheFirst) {
  connectAndGetAccepted();
  ASSERT_TRUE(chat().send(FIRST_TEXT));
  ASSERT_TRUE(chat().send(SECOND_TEXT));
  EXPECT_EQ(sentLines().size(), 1U);

  relayLine(FIRST_LINE, FIRST_TEXT, ACCEPTED_PLAYER_NUMBER);

  ASSERT_EQ(sentLines().size(), 2U);
  EXPECT_EQ(sentLines().back(),
            (network::ChatLine{.lineNumber = SECOND_LINE,
                               .text = std::string{SECOND_TEXT}}));
}

/**
 * Given a line sent and never relayed back
 * When five seconds go by in frames of 100 ms
 * Then ten copies left, and a line of the client says it got no answer
 */
TEST_F(ChatChannelFixture, AfterFiveSecondsTheLineGetsNoAnswer) {
  connectAndGetAccepted();
  ASSERT_TRUE(chat().send(FIRST_TEXT));

  for (int frame = 0; frame < FRAMES_UNTIL_GIVING_UP; ++frame) {
    update(SHORT_FRAME);
  }

  EXPECT_EQ(sentLines().size(), COPIES_BEFORE_GIVING_UP);
  ASSERT_FALSE(chat().entries().empty());
  EXPECT_EQ(chat().entries().back().playerNumber, CLIENT_WRITER);
  EXPECT_EQ(chat().entries().back().text, UNANSWERED_CHAT_TEXT);
}

/**
 * Given one line in flight and three waiting
 * When a fifth line is sent
 * Then it is refused, and a line of the client says to wait
 */
TEST_F(ChatChannelFixture, AFifthLineIsRefused) {
  connectAndGetAccepted();
  for (int line = 0; line < LINES_TAKEN_AT_ONCE; ++line) {
    ASSERT_TRUE(chat().send(FIRST_TEXT));
  }

  EXPECT_FALSE(chat().send(FIRST_TEXT));
  ASSERT_FALSE(chat().entries().empty());
  EXPECT_EQ(chat().entries().back().text, CHAT_QUEUE_FULL_TEXT);
}

/**
 * Given NOVA listed as player 2 in the waiting room
 * When the server relays a line of player 2
 * Then the chat holds it, named NOVA, just arrived
 */
TEST_F(ChatChannelFixture, ARelayedLineIsNamedByTheWaitingRoom) {
  connectAndGetAccepted();
  room().setLobbyPlayers({network::LobbyPlayer{.playerNumber = NOVA_NUMBER,
                                               .ready = false,
                                               .nickname = std::string{NOVA}}});

  relayLine(NOVA_LINE, SECOND_TEXT, NOVA_NUMBER);

  ASSERT_EQ(chat().entries().size(), 1U);
  EXPECT_EQ(chat().entries().front(),
            (ChatEntry{.playerNumber = NOVA_NUMBER,
                       .nickname = std::string{NOVA},
                       .text = std::string{SECOND_TEXT},
                       .age = {}}));
}

/**
 * Given 33 lines relayed
 * When the chat is read
 * Then it holds the 32 newest, line 2 first
 */
TEST_F(ChatChannelFixture, TheChatKeepsThirtyTwoLines) {
  connectAndGetAccepted();
  for (int line = 1; line <= LINES_PAST_THE_LIMIT; ++line) {
    relayLine(static_cast<std::uint16_t>(line), "line " + std::to_string(line),
              NOVA_NUMBER);
  }

  ASSERT_EQ(chat().entries().size(), LINES_KEPT);
  EXPECT_EQ(chat().entries().front().text, OLDEST_KEPT);
}

/**
 * Given a line relayed
 * When half a second goes by
 * Then the line is half a second old
 */
TEST_F(ChatChannelFixture, LinesAgeWithTheSession) {
  connectAndGetAccepted();
  relayLine(NOVA_LINE, SECOND_TEXT, NOVA_NUMBER);

  update(CHAT_INTERVAL);

  ASSERT_EQ(chat().entries().size(), 1U);
  EXPECT_EQ(chat().entries().front().age, CHAT_INTERVAL);
}

/**
 * Given a line relayed
 * When the session closes
 * Then the chat is empty, and a line can no longer be sent
 */
TEST_F(ChatChannelFixture, TheEndOfTheSessionForgetsTheChat) {
  connectAndGetAccepted();
  relayLine(NOVA_LINE, SECOND_TEXT, NOVA_NUMBER);

  chat().sessionClosed();

  EXPECT_TRUE(chat().entries().empty());
  EXPECT_FALSE(chat().send(FIRST_TEXT));
}

/**
 * Given an open session
 * When an empty line, then a line of 128 bytes, is sent
 * Then both are refused: the protocol takes 1 to 127 bytes
 */
TEST_F(ChatChannelFixture, ATextTheProtocolRefusesIsRefused) {
  connectAndGetAccepted();

  EXPECT_FALSE(chat().send(""));
  EXPECT_FALSE(chat().send(std::string(TOO_LONG_TEXT_SIZE, FILLER_LETTER)));
}

}  // namespace rtype::client
