#include <gtest/gtest.h>
#include <cstdint>
#include <string>
#include <vector>
#include "ChatLine.hpp"
#include "ChatRelayFixture.hpp"
#include "ChatRelayTestConstants.hpp"
#include "ConnectedPlayer.hpp"
#include "PacketHeader.hpp"
#include "PlayerChatLine.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

using network::ChatLine;
using network::PlayerChatLine;

/**
 * Given two seated players
 * When the first one sends a line
 * Then each of them, its writer included, gets that line under the number of
 * its writer
 */
TEST_F(ChatRelayFixture, RelaysALineToEveryPlayerItsWriterIncluded) {
  const ConnectedPlayer& writer = seat(FIRST_SEAT);
  seat(SECOND_SEAT);

  say(FIRST_SEAT, SOME_LINE_NUMBER);

  const PlayerChatLine expected{
      .line = ChatLine{.lineNumber = SOME_LINE_NUMBER,
                       .text = std::string{SOME_TEXT}},
      .playerNumber = writer.playerNumber};
  EXPECT_EQ(linesTo(FIRST_SEAT), std::vector<PlayerChatLine>{expected});
  EXPECT_EQ(linesTo(SECOND_SEAT), std::vector<PlayerChatLine>{expected});
}

/**
 * Given a seated player
 * When it sends a line
 * Then the datagram that carries it is marked guaranteed: it is sent again
 * until acknowledged
 */
TEST_F(ChatRelayFixture, SendsTheLineGuaranteed) {
  seat(FIRST_SEAT);

  say(FIRST_SEAT, SOME_LINE_NUMBER);

  ASSERT_EQ(sent().size(), 1U);
  EXPECT_NE(headerOf(sent().at(0)).flags & network::GUARANTEED_FLAG, 0U);
}

/**
 * Given a line relayed, then the whole interval gone by
 * When its client sends that line again, under the same number
 * Then nothing more is relayed: it is a copy
 */
TEST_F(ChatRelayFixture, IgnoresACopyOfALineItRelayed) {
  seat(FIRST_SEAT);
  say(FIRST_SEAT, SOME_LINE_NUMBER);
  runTicks(TICKS_OF_THE_CHAT_INTERVAL);

  say(FIRST_SEAT, SOME_LINE_NUMBER);

  EXPECT_EQ(linesTo(FIRST_SEAT).size(), 1U);
}

/**
 * Given the line 7 relayed, then the whole interval gone by
 * When the line 6 arrives
 * Then nothing more is relayed: a number before the newest is an old copy
 */
TEST_F(ChatRelayFixture, IgnoresALineNumberedBeforeTheNewest) {
  seat(FIRST_SEAT);
  say(FIRST_SEAT, SOME_LINE_NUMBER);
  runTicks(TICKS_OF_THE_CHAT_INTERVAL);

  say(FIRST_SEAT, EARLIER_LINE_NUMBER);

  EXPECT_EQ(linesTo(FIRST_SEAT).size(), 1U);
}

/**
 * Given a line relayed just now
 * When the next line arrives 59 ticks later, then again 60 ticks later
 * Then the first is ignored and the second relayed: the two sides of the
 * interval of the protocol page
 */
TEST_F(ChatRelayFixture, RelaysOneLinePerIntervalAndTheNextOnceItIsOver) {
  seat(FIRST_SEAT);
  say(FIRST_SEAT, SOME_LINE_NUMBER);

  runTicks(TICKS_ONE_SHORT_OF_THE_CHAT_INTERVAL);
  say(FIRST_SEAT, NEXT_LINE_NUMBER);
  EXPECT_EQ(linesTo(FIRST_SEAT).size(), 1U);

  runTicks(1);
  say(FIRST_SEAT, NEXT_LINE_NUMBER);
  EXPECT_EQ(linesTo(FIRST_SEAT).size(), 2U);
}

/**
 * Given a line of the first player relayed just now
 * When the second player sends a line in the same tick
 * Then it is relayed: a flood costs its writer only
 */
TEST_F(ChatRelayFixture, CountsTheIntervalPerPlayer) {
  seat(FIRST_SEAT);
  seat(SECOND_SEAT);
  say(FIRST_SEAT, SOME_LINE_NUMBER);

  say(SECOND_SEAT, SOME_LINE_NUMBER);

  EXPECT_EQ(linesTo(FIRST_SEAT).size(), 2U);
}

/**
 * Given the line 65535 relayed, then the whole interval gone by
 * When the line 0 arrives
 * Then it is relayed: the number wraps from 65535 to 0
 */
TEST_F(ChatRelayFixture, TakesTheLineAfterTheWrapForANewOne) {
  seat(FIRST_SEAT);
  say(FIRST_SEAT, LAST_LINE_NUMBER_BEFORE_THE_WRAP);
  runTicks(TICKS_OF_THE_CHAT_INTERVAL);

  say(FIRST_SEAT, FIRST_LINE_NUMBER_AFTER_THE_WRAP);

  EXPECT_EQ(linesTo(FIRST_SEAT).size(), 2U);
}

/**
 * Given a player whose line 7 was relayed just now, who then leaves
 * When a new player, from another address, takes the freed seat and sends its
 * line 1 in the same tick
 * Then it is relayed: neither the number nor the wait of the seat passes to the
 * next player
 */
TEST_F(ChatRelayFixture, ANewPlayerOnAFreedSeatStartsAfresh) {
  const std::uint8_t freedSeat = seat(FIRST_SEAT).playerNumber;
  say(FIRST_SEAT, SOME_LINE_NUMBER);
  leave(FIRST_SEAT);
  ASSERT_EQ(seat(SECOND_SEAT).playerNumber, freedSeat);

  say(SECOND_SEAT, FIRST_LINE_NUMBER);

  EXPECT_EQ(linesTo(SECOND_SEAT).size(), 1U);
}

}  // namespace rtype::server::testing
